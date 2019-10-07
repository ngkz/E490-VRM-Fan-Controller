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

#include "debug.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdbool.h>
#include "thermometer.h"
#include "fan.h"

bool trace;

void debug_ui(void) {
    for (;;) {
        printf_P(PSTR("? "));
        char ch = getchar();
        putchar('\n');
        switch(ch) {
            case 'T':
                //trace on
                trace = true;
                break;
            case 't':
                //trace off
                trace = false;
                break;
            case 'n':
                diode_on_10uA();
                break;
            case 'N':
                diode_on_200uA();
                break;
            case 'f':
                diode_off();
                break;
            case 'v':
                // read diode voltage
                printf_P(PSTR("%fmV\n"), read_diode_voltage() * 1000);
                break;
            case 'd': {
                //set fan duty
                uint8_t duty = 0;
                printf_P(PSTR("duty? "));
                scanf_P(PSTR("%hhu"), &duty);
                set_fan_duty(duty);
                break;
            }
            case 'q':
                goto cleanup;
            default:
                puts_P(PSTR("E"));
                break;
        }
    }

cleanup:
    diode_off();
    set_fan_duty(0);
}

void tracef_P(const char *format /* PROGMEM */, ...) {
    if (trace) {
        va_list ap;

        va_start(ap, format);
        vfprintf_P(stdout, format, ap);
        va_end(ap);
    }
}
