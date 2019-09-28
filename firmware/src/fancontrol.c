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

#include <stdbool.h>
#include <math.h>
#include <avr/pgmspace.h>
#include "config.h"
#include "fancontrol.h"
#include "fan.h"
#include "thermometer.h"
#include "uart.h"

static uint8_t duty;
static uint16_t fg_wait_remain;
static bool trace;

void reset_fan_control() {
    duty = 0;
    fg_wait_remain = 0;
}

void fan_control_loop(int control_period) {
    uint16_t rpm = tachometer_capture(control_period);
    if (trace) putu(rpm);

    if (fg_wait_remain > 0) {
        if (fg_wait_remain > control_period) {
            fg_wait_remain -= control_period;
        } else {
            fg_wait_remain = 0;
        }

        if (trace) {
            putch(','); putu(fg_wait_remain);
        }

        if (fg_wait_remain == 0) {
            // FG ready
            tachometer_start();
            if (trace) putch('A');
        }
    }

    uint8_t next_duty;
    int8_t temp = measure_temp();
    if (trace) {
        putch(','); putd(temp);
    }

    if (temp <= config.fan_stop_temp) {
        next_duty = 0;
        if (trace) putP(PSTR("B1"));
    } else if (config.fan_stop_temp < temp && temp < config.fan_start_temp) {
        if (duty == 0) {
            next_duty = 0;
            if (trace) putP(PSTR("B2"));
        } else {
            next_duty = config.min_duty;
            if (trace) putP(PSTR("B3"));
        }
    } else if (config.fan_start_temp <= temp && temp < config.fan_full_speed_temp) {
        next_duty = (uint8_t)roundf((float)(config.max_duty - config.min_duty) /
                                           (config.fan_full_speed_temp - config.fan_start_temp) *
                                                (temp - config.fan_start_temp)) + config.min_duty;
            if (trace) putP(PSTR("B4"));
    } else { //temp >= config.fan_full_speed_temp
        next_duty = config.max_duty;
        if (trace) putP(PSTR("B5"));
    }

    // ensure fan runs at least startup duty when FG signal is indeterminate
    if (duty == 0 && next_duty > 0) {
        // fan startup
        fg_wait_remain = config.fg_delay;
        if (trace) putP(PSTR("C"));
    }

    if (0 < next_duty && next_duty < config.startup_duty &&
            (fg_wait_remain > 0 /* FG not ready, tachometer stopped */ ||
             rpm < config.min_rpm /* fan stalled */)) {
        next_duty = config.startup_duty;
        if (trace) putP(PSTR("D"));
    }

    if (duty > 0 && next_duty == 0) {
        // fan stop
        tachometer_stop();
        if (trace) putP(PSTR("E"));
    }

    set_fan_duty(next_duty);
    duty = next_duty;

    if (trace) {
        putch(',');
        putuln(duty);
    }
}

void stop_fan_control() {
    set_fan_duty(0);
    tachometer_stop();
}

void toggle_fan_control_trace() {
    trace = !trace;
}
