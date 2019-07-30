#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <util/delay.h>

#define PWM_N PB4
#define FG    PB3

FUSES = {
    // CKDIV8 = 0          -> Clock divided by 8 (1MHz system clock when CKSEL = 0010)
    // CKOUT = 1           -> Clock output disabled
    // SUT[1:0] = 10       -> Start-up time is 14 clock + 64ms (when CKSEL = 0010)
    // CKSEL[3:0] = 0010   -> Internal RC Oscillator provides 8MHz clock
    .low = LFUSE_DEFAULT,
    // RSTDISBL = 1        -> External reset pin enabled (needed for low voltage serial programming)
    // DWRN = 1            -> DebugWIRE disabled
    // SPIEN = 0           -> Serial programming enabled
    // WDTON = 1           -> Watchdog timer enabled on boot
    // EESAVE = 1          -> Chip erase erases EEPROM
    // BODLEVEL[2:0] = 111 -> Brown-out Detector disabled
    .high = HFUSE_DEFAULT,
    // SELFPRGEN = 1       -> Self-programming enabled
    .extended = EFUSE_DEFAULT
};

volatile unsigned long cnt_256us;

ISR(TIM0_OVF_vect) {
    cnt_256us++;
}

//duty: 0-159
void set_fan_duty(uint8_t duty) {
    if (duty > 0) {
        //Reset Timer 1 counter
        TCNT1 = 0;
        //COM1B[1:0] = 11 -> Connect pulse width modulator B to OC1B(PWM#).
        //                   OC1B(PB4) set on compare match. Cleared when TCNT1 = 0
        //PSR1 = 1        -> Reset Timer 1 prescaler
        GTCCR |= _BV(COM1B1) | _BV(COM1B0) | _BV(PSR1);
        //Set PWM duty
        OCR1B = duty;
        //Turn on timer 1
        PRR &= ~_BV(PRTIM1);

    } else {
        //Cut clock to Timer 1
        PRR |= _BV(PRTIM1);
        //COM1B[1:0] = 00 -> Disonnect OC1B(PWM#)
        GTCCR &= ~_BV(COM1B1) & ~_BV(COM1B0);
        //Turn off the fan.
        PORTB |= _BV(PWM_N);
    }
}

int main(void) {
    //Port configuration
    //Set PWM#(PB4) as output
    DDRB = _BV(PWM_N);
    //Pull-up unconnected PB1 pin to avoid high power consumption
    //Pull-up open-drain input pin FG
    PORTB |= _BV(PB1) | _BV(FG);
    //Turn off the fan.
    PORTB |= _BV(PWM_N);

    //Disable unneeded modules to reduce power consumption
    //Disable analog comparator
    ACSR = _BV(ACD);
    //Cut clock to Timer1 and ADC
    PRR = _BV(PRTIM1) | _BV(PRADC);

    //Timer0 configuration
    //Enable overflow interrupt
    TIMSK = _BV(TOIE0);
    //Start the timer, no prescaling (1MHz clock)
    TCCR0B = _BV(CS00);

    //Timer1 configuration
    //Enable the PLL
    //LSM = 0  -> High speed mode (64MHz)
    //PCKE = 0 -> Timer/Counter 1 clock source is system clock
    //PLLE = 1 -> Enable PLL
    PLLCSR = _BV(PLLE);
    //Wait for PLL to stabilize
    _delay_us(100);
    //Wait for the PLL to lock
    while(!PLLCSR & _BV(PLOCK));
    //PCKE = 1 -> Use 64MHz PLL clock as the Timer1 clock source.
    PLLCSR |= _BV(PCKE);
    //PWM1A = 0             -> Disable pulse width modulator A
    //COM1A[1:0] = 00       -> OC1A and #OC1A are not connected
    //CS1[3:0] = 0101       -> Timer1 clock is 64MHz from PLL/16 = 4MHz
    TCCR1 = _BV(CS12) | _BV(CS10);
    //PWM1B = 1             -> Enable pulse width modulator B
    //COM1B[1:0] = 00       -> OC1B(PWM#) and #OC1B are not connected. set_fan_duty() connects them when neeeded.
    //FOC1B = 0
    //FOC1A = 0
    //PSR1 = 0
    //PSR0 = 0
    GTCCR = _BV(PWM1B);
    //PWM freqency 4MHz / (159 + 1) = 25kHz
    OCR1C = 159;

    //Enable interrupt
    sei();

    set_fan_duty(79);

    //Stop CPU core until an interrupt occurs.
    for (;;) sleep_mode();
}
