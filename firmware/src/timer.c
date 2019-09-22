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

#include "timer.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/power.h>

static volatile bool tick;

void init_timer() {
    GTCCR &= ~(_BV(TSM) | _BV(PSR0));
    // disconnect OC0A and OC0B, CTC mode
    TCCR0A = _BV(WGM01);
    // reset counter
    TCNT0 = 0;
    // 500ms period
    OCR0A = 244;
    OCR0B = 0;
    // no force output compare match, timer 0 stopped
    TCCR0B = 0;
    // enable Timer/Counter0 output compare match A interrupt, disable output compare match B interrupt and overflow interrupt
    TIMSK = (TIMSK & ~(_BV(OCIE0A) | _BV(OCIE0B) | _BV(TOIE0))) | _BV(OCIE0A);
    // clear the Timer1 interrupt flags
    TIFR |= _BV(OCF0A) | _BV(OCF0B) | _BV(TOV0);

    power_timer0_disable();
}

void start_timer() {
    clear_elapsed_flag();
    power_timer0_enable();
    // reset counter
    TCNT0 = 0;
    // reset timer 0 prescaler
    GTCCR |= _BV(PSR0);
    // no force output compare match, start timer 0, 125kHz / 256 = 488.2Hz clock
    TCCR0B = _BV(CS02);
}

void stop_timer() {
    // no force output compare match, timer 0 stopped
    TCCR0B = 0;

    power_timer0_disable();
}

ISR(TIM0_COMPA_vect) {
    tick = true;
}

bool is_timer_elapsed() {
    return tick;
}

void clear_elapsed_flag() {
    tick = false;
}