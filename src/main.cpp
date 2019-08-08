#include <Arduino.h>
#include <Wire.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/power.h>

#define P_PWM_N          4 //PWM#
#define P_FG             3
#define P_UNUSED         1
#define SMBUS_SLAVE_ADDR 0x12

enum {
    REG_DUTY,
    REG_RPM
};

volatile unsigned long lastLevelChangeTime;
volatile unsigned long levelChangeInterval;
// current register address for read/write
volatile uint8_t addr = 0;

ISR(PCINT0_vect) {
    unsigned long currentTime = micros();
    // 3/4*levelChangeInterval + 1/4*(currentTime - lastLevelChangeTime)
    levelChangeInterval = levelChangeInterval - (levelChangeInterval >> 2) + ((currentTime - lastLevelChangeTime) >> 2);
    lastLevelChangeTime = currentTime;
}

uint8_t getFanDuty() {
    return GTCCR & (_BV(COM1B1) | _BV(COM1B0)) /* PWM B running */ ? OCR1B : 0;
}

// duty: 0-159
void setFanDuty(uint8_t duty) {
    if (duty > 0) {
        power_timer1_enable();
        // reset the counter
        TCNT1 = 0;
        // set the duty
        OCR1B = duty;
        // connect PWM B to OC1B(PWM#), active low, reset timer 1 prescaler
        GTCCR |= _BV(COM1B1) | _BV(COM1B0) | _BV(PSR1);
        // start timer 1, 64MHz / 16 = 4MHz clock
        TCCR1 |= _BV(CS12) | _BV(CS10);
    } else {
        // disconnect PWM B from PWM# pin
        GTCCR &= ~_BV(COM1B1) & ~_BV(COM1B0);
        // stop timer 1
        TCCR1 &= ~_BV(CS13) & ~_BV(CS12) & ~_BV(CS11) & ~_BV(CS10);
        power_timer1_disable();

        digitalWrite(P_PWM_N, HIGH);
    }
}

// function that executes whenever data is received from master
void receiveEvent(int howMany) {
    if (howMany <= 0) return;

    // read register address
    addr = Wire.read();
    howMany--;

    if (addr == REG_DUTY && howMany >= 1) {
        // duty write request
        setFanDuty(Wire.read());
    }

    // clear rx buffer
    while (Wire.available()) Wire.read();
}

// function that executes whenever data is requested by master
void requestEvent() {
    switch (addr) {
    case REG_DUTY:
        // duty read request
        Wire.write(getFanDuty());
        break;
    case REG_RPM:
        // rpm read request
        uint16_t rpm = 60 * 1000000 / 4 / levelChangeInterval;
        Wire.write(rpm);
        Wire.write(rpm >> 8);
        break;
    }
}

void setup() {
    // FG connected to open-drain output
    pinMode(P_FG, INPUT_PULLUP);
    // pull-up unconnected pins to avoid high power consumption
    pinMode(P_UNUSED, INPUT_PULLUP);
    // PWM# is active low
    pinMode(P_PWM_N, OUTPUT);
    digitalWrite(P_PWM_N, HIGH);

    // disable unneeded modules to reduce power consumption
    // ADC and analog comparator
    ACSR &= ~_BV(ACD);
    ADCSRA &= ~_BV(ADEN);
    power_adc_disable();

    // timer 1 configuration
    // no clear on compare match, turn off PWM A, disconnect timer comparator A from the output pin, stop the clock
    TCCR1 = 0;
    // turn on PWM B, disconnect PWM B from the output pin, no force output compare match, no timer 1 prescaler reset
    GTCCR = (GTCCR | _BV(PWM1B)) & ~(_BV(COM1B1) | _BV(COM1B0) | _BV(FOC1B) | _BV(FOC1A) | _BV(PSR1));
    // reset the counter
    TCNT1 = 0;
    // reset the output compare registers
    OCR1A = 0;
    OCR1B = 0;
    // PWM freqency 4MHz / (159 + 1) = 25kHz, setFanDuty() configures the prescaler.
    OCR1C = 159;
    // Disable all Timer1 interrupts
    TIMSK &= ~(_BV(OCIE1A) | _BV(OCIE1B) | _BV(TOIE1));
    // Clear the Timer1 interrupt flags
    TIFR |= _BV(OCF1A) | _BV(OCF1B) | _BV(TOV1);

    power_timer1_disable();

    // generate PCINT0 interrupt when PCINT3(P_FG) is changed
    PCMSK = _BV(PCINT3);
    GIMSK |= _BV(PCIE);

    Wire.begin(SMBUS_SLAVE_ADDR);
    Wire.onRequest(requestEvent);
    Wire.onReceive(receiveEvent);

    set_sleep_mode(SLEEP_MODE_IDLE);
}

void loop() {
    sleep_mode();
}
