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
#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include "fan.h"
#include "thermometer.h"
#include "uart.h"
#include "debug.h"
#include "config.h"

int main() {
    init_uart();
    init_fan();
    init_thermometer();

    power_timer0_disable();
    power_usi_disable();

    //pull-up unused pin to avoid high power consumptio
    DDRB &= ~(_BV(PB2));
    PORTB |= _BV(PB2);

    //Wake AVR from sleep every 500ms
    //Enable watchdog interrupt, typical timeout 0.5s
    WDTCR = _BV(WDIE) | _BV(WDP2) | _BV(WDP0);

    sei();

    uint8_t duty = 0;

    for (;;) {
        TRACE("FC: ");

        int8_t temp = measure_temp();
        uint8_t next_duty;

        if (temp <= T_FAN_STOP) {
            TRACE("T <= T_FAN_STOP");
            next_duty = 0;
        } else if (T_FAN_STOP <= temp && temp < T_FAN_START) {
            TRACE("T_FAN_STOP <= T < T_FAN_START");
            if (duty == 0) {
                TRACE(", fan stopped");
                next_duty = 0;
            } else {
                TRACE(", fan running");
                next_duty = MIN_DUTY;
            }
        } else if (T_FAN_START <= temp && temp < T_FAN_FULL_SPEED) {
            TRACE("T_FAN_START <= T < T_FAN_FULL_SPEED");
            next_duty = (uint8_t)roundf((float)(MAX_DUTY - MIN_DUTY) /
                                               (T_FAN_FULL_SPEED - T_FAN_START) *
                                               (temp - T_FAN_START)) + MIN_DUTY;
        } else { //temp >= T_FAN_FULL_SPEED
            TRACE("T >= T_FAN_FULL_SPEED");
            next_duty = MAX_DUTY;
        }


        TRACE(", duty=%u", next_duty);

        set_fan_duty(next_duty);
        duty = next_duty;

        //wait for next wdt interrupt
        if (next_duty == 0) {
            TRACE(", power down\n", next_duty);
            set_sleep_mode(SLEEP_MODE_PWR_DOWN);
        } else {
            TRACE(", idle\n", next_duty);
            //Timer1 stops when in power-down mode
            set_sleep_mode(SLEEP_MODE_IDLE);
        }
        sleep_mode();
    }

    return 0;
};

EMPTY_INTERRUPT(WDT_vect);
