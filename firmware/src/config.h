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

#ifndef __CONFIG_H_
#define __CONFIG_H_

#include <stdint.h>

struct Config {
    uint16_t zero_c_voltage;
    float    temperature_coefficient;
    uint8_t  min_duty;
    uint8_t  max_duty;
    uint8_t  startup_duty;
    int8_t   fan_start_temp;
    int8_t   fan_full_speed_temp;
    int8_t   fan_stop_temp;
    uint8_t  pulse_per_revolution;
    uint16_t min_rpm;
    uint16_t fg_delay;
};

void load_config(struct Config *config);

#endif // __CONFIG_H_
