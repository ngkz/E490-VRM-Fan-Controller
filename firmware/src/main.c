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

#include <math.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include "config.h"
#include "uart.h"
#include "fan.h"
#include "thermometer.h"
#include "fancontrol.h"

#define TUNED_OSCCAL 0x99

int main() {
    // decrease clock speed
    clock_prescale_set(clock_div_64); // 8MHz / 64 = 125kHz

    // calibrate internal oscillator
    OSCCAL = TUNED_OSCCAL;

    init_config();
    init_uart_stdio();
    init_fan();
    init_thermometer();
    init_fan_control();

    power_usi_disable();

    sei();

    set_sleep_mode(SLEEP_MODE_IDLE);

    for (;;) {
        // control loop timer interrupt or pin change interrupt or FG rising interrupt
        fan_control();
        sleep_mode();
    }

    return 0;
};
