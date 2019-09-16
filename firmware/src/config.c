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

#include "config.h"
#include <avr/eeprom.h>

struct Config config_EE EEMEM = {
    .zero_c_voltage          = 0,
    .temperature_coefficient = 0.0,
    .min_duty                = 40,   // 25%
    .max_duty                = 159,  // 100%
    .startup_duty            = 48,   // 30%
    .fan_start_temp          = 45,   // C
    .fan_full_speed_temp     = 70,   // C
    .fan_stop_temp           = 40,   // C
    .pulse_per_revolution    = 2,
    .min_rpm                 = 1000,
    .fg_delay                = 1000, // ms
};

struct Config config;

static void load_config() {
    eeprom_busy_wait();
    eeprom_read_block(&config, &config_EE, sizeof(struct Config));
}

void init_config() {
    load_config();
}
