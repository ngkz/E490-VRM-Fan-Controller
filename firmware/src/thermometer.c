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
#include <avr/power.h>
#include <avr/io.h>
#include "thermometer.h"
#include "config.h"

#define P_DIODE      PB3 //D+
#define ADMUX_DIODE  0b11

static void diode_on() {
    // supply current to the diode through internal pull-up resistor
    DDRB &= ~_BV(P_DIODE);
    PORTB |= _BV(P_DIODE);
}

static void diode_off() {
    // power off the diode (output low)
    PORTB &= ~_BV(P_DIODE);
    DDRB |= _BV(P_DIODE);
}

static void enable_adc() {
    power_adc_enable();
    ADCSRA |= _BV(ADEN);
}

static void disable_adc() {
    ADCSRA &= ~_BV(ADEN);
    power_adc_disable();
}

void init_thermometer() {
    // use internal 1.1V voltage reference, no left adjust result, input is D+(PB3)
    ADMUX = _BV(REFS1) | ADMUX_DIODE << MUX0;
    // ADC disabled, Auto Trigger disabled, ADC interrupt disabled, prescaler 1/2 (75kHz)
    ADCSRA = 0;
    // unipolar input mode, analog comparator multiplexer disabled, no input polarity reversal
    ADCSRB = 0;
    // disable digital input buffer of D+(ADC3)
    DIDR0 = _BV(ADC3D);
    // analog comparator disabled
    ACSR = _BV(ACD);
    // shutdown adc and analog comparator
    power_adc_disable();

    diode_off();
}

static uint16_t adc() {
    // start conversion
    ADCSRA |= _BV(ADSC);
    // wait for conversion to complete
    loop_until_bit_is_clear(ADCSRA, ADSC);
    //ADCL must be read first
    uint8_t result_low = ADCL;
    uint8_t result_high = ADCH;
    return result_high << 8 | result_low;
}

uint16_t adc_diode_voltage() {
    enable_adc();
    diode_on();
    uint16_t voltage = (adc() + adc() + adc()) / 3;
    diode_off();
    disable_adc();
    return voltage;
}

int8_t measure_temp() {
    return (int8_t)roundf((adc_diode_voltage() - config.zero_c_voltage) /
                           config.temperature_coefficient);
}
