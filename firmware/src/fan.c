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
#include <avr/power.h>
#include <util/delay.h>
#include "fan.h"

#define P_PWM        PB1 //OC1A

static volatile uint8_t fg_pulse_count = 0;

void init_fan() {
    DDRB |= _BV(P_PWM);   // PWM out
    PORTB &= ~_BV(P_PWM); // PWM low

#if P_PWM != PB1
#error "fix timer 1 configuration"
#endif

    // timer 1 configuration
    // no clear on compare match, turn on PWM A, disconnect timer comparator A from the output pin, stop the clock
    TCCR1 = _BV(PWM1A);
    // turn off PWM B, disconnect PWM B from the output pin, no force output compare match, no timer 1 prescaler reset
    GTCCR &= ~(_BV(PWM1B) | _BV(COM1B1) | _BV(COM1B0) | _BV(FOC1B) | _BV(FOC1A) | _BV(PSR1));
    // reset the counter
    TCNT1 = 0;
    // reset output compare registers
    OCR1A = 0;
    OCR1B = 0;
    // PWM freqency 250kHz(Timer 1 clock) / (9 + 1) = 25kHz, setFanDuty() configures the clock.
    OCR1C = 9;
    // disable all Timer1 interrupts
    TIMSK &= ~(_BV(OCIE1A) | _BV(OCIE1B) | _BV(TOIE1));
    // clear the Timer1 interrupt flags
    TIFR |= _BV(OCF1A) | _BV(OCF1B) | _BV(TOV1);

    power_timer1_disable();
}

//duty: 0-OCR1C
void set_fan_duty(uint8_t duty) {
    if (duty > 0 && duty < OCR1C) {
        // start PWM
        power_timer1_enable();
        // set the duty
        OCR1A = duty;
        // reset the counter
        TCNT1 = 0;
        // reset timer 1 prescaler
        GTCCR |= _BV(PSR1);
        // connect PWM A to OC1A,  start timer 1, 250kHz clock
        TCCR1 |= _BV(COM1A1) | _BV(CS10);
    } else {
        // stop PWM
        // disconnect PWM A from OC1A stop timer 1
        TCCR1 &= ~(_BV(COM1A1) | _BV(COM1A0) | _BV(CS13) | _BV(CS12) | _BV(CS11) | _BV(CS10));
        power_timer1_disable();

        PORTB = (PORTB & ~_BV(P_PWM)) | (duty == 0 ? 0 : _BV(P_PWM));
    }
}
