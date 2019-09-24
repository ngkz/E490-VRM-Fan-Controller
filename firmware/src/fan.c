/*
 *  ThinkPad E490 VRM Cooling Mod Controller Firmware
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

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <util/delay.h>
#include "fan.h"
#include "config.h"

#define P_FG         PB2 //INT0
#define P_PWM        PB4

static volatile uint8_t fg_pulse_count = 0;

void init_fan() {
    DDRB |= _BV(P_PWM);   // PWM out
    PORTB &= ~_BV(P_PWM); // PWM low
    DDRB &= ~_BV(P_FG);   // FG input
    PORTB |= _BV(P_FG);   // pull-up FG

#if P_PWM != PB4
#error "fix timer 1 configuration"
#endif

    // timer 1 configuration
    // asynchronous mode
    // enable PLL
    PLLCSR |= _BV(PLLE);
    // wait 100us for PLL to stabilize
    _delay_us(100);
    // wait for PLL to lock
    loop_until_bit_is_set(PLLCSR, PLOCK);
    // Use 64MHz PLL clock as timer 1 clock source
    PLLCSR |= PCKE;
    // no clear on compare match, turn off PWM A, disconnect timer comparator A from the output pin, stop the clock
    TCCR1 = 0;
    // turn on PWM B, disconnect PWM B from the output pin, no force output compare match, no timer 1 prescaler reset
    GTCCR = (GTCCR & ~(_BV(PWM1B) | _BV(COM1B1) | _BV(COM1B0) | _BV(FOC1B) | _BV(FOC1A) | _BV(PSR1))) | _BV(PWM1B);
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

    // the rising edge of FG(INT0) generates an INT0 interrupt
    MCUCR |= _BV(ISC01) | _BV(ISC00);
}

void set_fan_duty(uint8_t duty) {
    if (duty > 0 && duty < OCR1C) {
        // start PWM
        power_timer1_enable();
        // set the duty
        OCR1A = duty;
        // reset the counter
        TCNT1 = 0;
        // connect PWM B to OC1B, reset timer 1 prescaler
        GTCCR |= _BV(COM1B1) | _BV(PSR1);
        // start timer 1, 64MHz / 16 = 4MHz clock
        TCCR1 |= _BV(CS12) | _BV(CS10) | _BV(COM1A1) /* workaround for ATTiny85 bug */;
    } else {
        // stop PWM
        // disconnect PWM B from OC1B
        GTCCR &= ~(_BV(COM1B1) | _BV(COM1B0));
        // stop timer 1
        TCCR1 &= ~(_BV(CS13) | _BV(CS12) | _BV(CS11) | _BV(CS10) | _BV(COM1A1) | _BV(COM1A0));
        power_timer1_disable();

        PORTB = (PORTB & ~_BV(P_PWM)) | (duty == 0 ? 0 : _BV(P_PWM));
    }
}

void tachometer_start() {
    // enable INT0 interrupt
    fg_pulse_count = 0;
    GIFR |= _BV(INTF0);
    GIMSK |= _BV(INT0);
}

void tachometer_stop() {
    // disable INT0 interrupt
    GIMSK &= ~_BV(INT0);
}

ISR(INT0_vect) {
    fg_pulse_count++;
}

uint16_t tachometer_capture(int capture_period) {
    uint16_t rpm = fg_pulse_count / config.pulse_per_revolution * 60000 / capture_period;
    fg_pulse_count = 0;
    return rpm;
}
