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

#include <stdint.h>
#include <stdio.h>
#include <avr/pgmspace.h>
#include "fancontrol.h"
#include "fan.h"
#include "thermometer.h"

static uint8_t current_level_idx;

struct level {
    uint8_t duty;
    int8_t lower_limit;
    int8_t upper_limit;
};

static const struct level fan_control_table[] = {
    {0, 0,  44},
    {3, 40, 47},
    {4, 43, 51},
    {5, 47, 54},
    {6, 50, 57},
    {7, 53, 60},
    {8, 56, 64},
    {9, 60, 99},
};
#define N_FAN_CONTROL_LEVELS (sizeof(fan_control_table) / sizeof(fan_control_table[0]))

#ifdef TRACE_ON
#define TRACE(format, ...) printf_P(PSTR(format), __VA_ARGS__)
#else
#define TRACE(format, ...)
#endif

void control_fan(void) {
    int8_t temp = measure_temp();
    TRACE("T=%d", temp);

    const struct level *current_level = &fan_control_table[current_level_idx];
    while (current_level_idx > 0 && temp < current_level->lower_limit) {
        TRACE(", T<%d", current_level->lower_limit);
        current_level--;
        current_level_idx--;
        TRACE(" lvl=%u", current_level_idx);
    }

    while (current_level_idx + 1 < N_FAN_CONTROL_LEVELS && temp > current_level->upper_limit) {
        TRACE(", T>%d", current_level->upper_limit);
        current_level++;
        current_level_idx++;
        TRACE(" lvl=%u", current_level_idx);
    }

    set_fan_duty(current_level->duty);

    TRACE(", duty=%u\n", current_level->duty);
}
