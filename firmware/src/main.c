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
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include "config.h"
#include "uart.h"
#include "fan.h"
#include "thermometer.h"
#include "timer.h"
#include "fancontrol.h"
#include "asciichr.h"

#define TUNED_OSCCAL 0x99

int main() {
    // decrease clock speed
    clock_prescale_set(clock_div_32); // 8MHz / 32 = 250kHz

    // calibrate internal oscillator
    OSCCAL = TUNED_OSCCAL;

    init_config();
    init_uart();
    init_fan();
    init_thermometer();
    init_timer();
    start_timer();

    //pull-up unused pin
    DDRB &= ~_BV(PB2);
    PORTB |= _BV(PB2);

    power_usi_disable();

    sei();

    set_sleep_mode(SLEEP_MODE_IDLE);

    for (;;) {
        while (available_input() > 0) {
            char ch = getch();
            if (ch == ASCII_CR || ch == ASCII_LF) {
                stop_fan_control();
                stop_timer();
                config_ui();
                start_timer();
                reset_fan_control();
                break;
            }
        }

        if (is_timer_elapsed()) {
            clear_elapsed_flag();
            fan_control_loop(TICK);
        }

        sleep_mode();
    }

    return 0;
};
