/*
 *  ThinkPad E490 VRM Fan Controller Firmware
 *  Copyright (C) 2019  Kazutoshi Noguchi
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include <Arduino.h>
#include <Wire.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/power.h>

#define P_PWM_N          1 //PWM#
#define P_FG             3
#define P_UNUSED         4
#define SMBUS_SLAVE_ADDR 0x12

enum {
    REG_DUTY,
    REG_RPM
};

volatile unsigned long lastLevelChangeTime = 0;
volatile unsigned long levelChangeInterval = 0;
// current register address for read/write
volatile uint8_t addr = 0;

ISR(PCINT0_vect) {
    unsigned long currentTime = micros();
    // 3/4*levelChangeInterval + 1/4*(currentTime - lastLevelChangeTime)
    levelChangeInterval = levelChangeInterval - (levelChangeInterval >> 2) + ((currentTime - lastLevelChangeTime) >> 2);
    lastLevelChangeTime = currentTime;
}

void setFanDuty(uint8_t duty) {
    if (duty > 0 || duty < 255) {
        power_timer1_enable();
        // reset the counter
        TCNT1 = 0;
        // set the duty
        OCR1B = (duty * OCR1C + 127) / 255;
        // reset timer 1 prescaler
        GTCCR |= _BV(PSR1);
        // connect PWM A to OC1A(PWM#), negative logic PWM, start timer 1, 64MHz / 16 = 4MHz clock
        TCCR1 |= _BV(COM1A1) | _BV(COM1A0) | _BV(CS12) | _BV(CS10);
    } else {
        // disconnect PWM A from PWM# pin, stop timer 1
        TCCR1 &= ~(_BV(COM1A1) | _BV(COM1A0) | _BV(CS13) | _BV(CS12) | _BV(CS11) | _BV(CS10));
        power_timer1_disable();
        //PWM# is negative logic
        digitalWrite(P_PWM_N, duty == 0 ? HIGH: LOW);
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
    if (addr == REG_RPM) {
        // rpm read request
        uint16_t rpm;
        if (micros() - lastLevelChangeTime < 1000000) {
            // >=15 RPM
            rpm = 60 * 1000000 / 4 / levelChangeInterval;
        } else {
            rpm = 0;
        }
        Wire.write(rpm);
        Wire.write(rpm >> 8);
    }
}

void setup() {
    // FG connected to open-drain output
    pinMode(P_FG, INPUT_PULLUP);
    // pull-up a floating pin to avoid high power consumption
    pinMode(P_UNUSED, INPUT_PULLUP);
    // PWM# is negative logic output
    pinMode(P_PWM_N, OUTPUT);
    digitalWrite(P_PWM_N, HIGH);

    // disable unneeded modules to reduce power consumption
    // ADC and analog comparator
    ACSR &= ~_BV(ACIE);
    ACSR |= _BV(ACD);
    ADCSRA &= ~_BV(ADEN);
    power_adc_disable();

    // timer 1 configuration
    // no clear on compare match, turn on PWM A, disconnect timer comparator A from the output pin, stop the clock
    TCCR1 = _BV(PWM1A);
    // turn off PWM B, disconnect PWM B from the output pin, no force output compare match, no timer 1 prescaler reset
    GTCCR &= ~(_BV(COM1B1) | _BV(COM1B0) | _BV(FOC1B) | _BV(FOC1A) | _BV(PSR1));
    // reset the counter
    TCNT1 = 0;
    // reset output compare registers
    OCR1A = 0;
    OCR1B = 0;
    // PWM freqency 4MHz(Timer 1 clock) / (159 + 1) = 25kHz, setFanDuty() configures the clock.
    OCR1C = 159;
    // disable all Timer1 interrupts
    TIMSK &= ~(_BV(OCIE1A) | _BV(OCIE1B) | _BV(TOIE1));
    // clear the Timer1 interrupt flags
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
