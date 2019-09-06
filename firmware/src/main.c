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
#include <stdbool.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include "config.h"
#include "uart.h"
#include "fan.h"
#include "thermometer.h"

#define TUNED_OSCCAL 0x99
#define TICK         512 //control loop timer period [ms]

void init_control_loop_timer() {
    GTCCR &= ~(_BV(TSM) | _BV(PSR0));
    // disconnect OC0A and OC0B, CTC mode
    TCCR0A = _BV(WGM01);
    // reset counter
    TCNT0 = 0;
    // 512ms period
    OCR0A = 249;
    OCR0B = 0;
    // no force output compare match, start timer 0, 125kHz / 256 = 488.2Hz clock
    TCCR0B = _BV(CS02) | _BV(CS00);
    // enable Timer/Counter0 output compare match A interrupt, disable output compare match B interrupt and overflow interrupt
    TIMSK = (TIMSK & ~(_BV(OCIE0A) | _BV(OCIE0B) | _BV(TOIE0))) | _BV(OCIE0A);
    // clear the Timer1 interrupt flags
    TIFR |= _BV(OCF0A) | _BV(OCF0B) | _BV(TOV0);
}

volatile bool tick = false;

ISR(TIM0_COMPA_vect) {
    tick = true;
}

int main() {
    // decrease clock speed
    clock_prescale_set(clock_div_64); // 8MHz / 64 = 125kHz

    // calibrate internal oscillator
    OSCCAL = TUNED_OSCCAL;

    struct Config config;
    load_config(&config);

    init_uart_stdio();
    init_fan();
    init_thermometer();
    init_control_loop_timer();

    power_usi_disable();

    sei();

    set_sleep_mode(SLEEP_MODE_IDLE);

    uint8_t duty = 0;
    uint16_t startup_wait_remain = 0;

    for (;;) {
        if (tick) {
            //control loop
            tick = false;

            if (startup_wait_remain > TICK) {
                startup_wait_remain -= TICK;
            } else {
                // startup complete
                startup_wait_remain = 0;
                start_tachometer();
            }

            int8_t temp = measure_vrm_temp(&config);
            uint8_t next_duty = 0;

            if (temp <= config.fan_stop_temp) {
                next_duty = 0;
            } else if (config.fan_stop_temp < temp && temp < config.fan_start_temp) {
                if (duty == 0) {
                    next_duty = 0;
                } else {
                    next_duty = config.min_duty;
                }
            } else if (config.fan_start_temp <= temp && temp < config.fan_full_speed_temp) {
                next_duty = (uint8_t)roundf((float)(config.max_duty - config.min_duty) /
                                                   (config.fan_full_speed_temp - config.fan_start_temp) *
                                                       (temp - config.fan_start_temp)) + config.min_duty;
            } else if (temp >= config.fan_full_speed_temp) {
                next_duty = config.max_duty;
            }

            if (duty == 0 && next_duty > 0) {
                // fan startup
                startup_wait_remain = config.fg_delay;
            }

            if (0 < next_duty && next_duty < config.startup_duty &&
                    (startup_wait_remain > 0 /* starting up */ ||
                     read_fan_rpm(&config, TICK) < config.min_rpm /* fan stalled */)) {
                next_duty = config.startup_duty;
            }

            if (duty > 0 && next_duty == 0) {
                // fan stop
                stop_tachometer();
            }

            set_fan_duty(next_duty);
            duty = next_duty;
            reset_tachometer();
        }

        sleep_mode();
    }

    return 0;
};
