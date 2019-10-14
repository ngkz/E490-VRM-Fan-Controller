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

#include <stdio.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include "fan.h"
#include "thermometer.h"
#include "timer.h"
#include "fancontrol.h"
#include "uart.h"
#include "debug.h"

#define TUNED_OSCCAL 0x99

int main() {
    // decrease clock speed
    clock_prescale_set(clock_div_32); // 8MHz / 32 = 250kHz

    // calibrate internal oscillator
    OSCCAL = TUNED_OSCCAL;

    init_uart();
    init_fan();
    init_thermometer();
    init_timer();

    power_usi_disable();

    //pull-up unused pin
    DDRB &= ~(_BV(PB2) | _BV(PB5));
    PORTB |= _BV(PB2) | _BV(PB5);

    sei();

    for (;;) {
        if (is_timer_elapsed()) {
            clear_elapsed_flag();
            control_fan();
        }

        while (available_input() > 0) {
            char ch = getchar();
            if (ch == '\r' || ch == '\n') debug_ui();
        }

        set_sleep_mode(SLEEP_MODE_IDLE);
        sleep_mode();
    }

    return 0;
};
