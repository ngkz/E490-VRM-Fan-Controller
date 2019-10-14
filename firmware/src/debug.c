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
#include <ctype.h>
#include "thermometer.h"
#include "fan.h"

bool trace;

static uint8_t readu8(void) {
    uint8_t value = 0;
    for (;;) {
        char ch = getchar();
        if (!isdigit(ch)) return value;
        value = value * 10 + ch - '0';
    }
}

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
            case 'v':
                printf_P(PSTR("%u LSB\n"), read_thermistor_voltage());
                break;
            case 'r': {
                bool trace_save = trace;
                trace = true;
                measure_temp();
                trace = trace_save;
                break;
            }
            case 'd': {
                //set fan duty
                printf_P(PSTR("duty? "));
                set_fan_duty(readu8());
                putchar('\n');
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
