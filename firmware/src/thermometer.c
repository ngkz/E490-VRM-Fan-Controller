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
#include "debug.h"

#define BOLTZMANN_CONSTANT 1.379553e-23f
#define ELEMENTARY_CHARGE  1.602177e-19f

#define P_BDPWR10          PB5
#define P_BDPWR200         PB2
#define P_DPLUS            PB4    //D+, ADC2
#define DID_DPLUS          ADC2D
#define P_DMINUS           PB3    //D-, ADC3
#define DID_DMINUS         ADC3D
#define ADMUX_DIODE        0b0110 //D+(ADC2) - D-(ADC3), 1x
#define CURRENT_RATIO      (1.0f/20)
#define IDEALITY_FACTOR    1.0f

void diode_on_10uA(void) {
    PORTB = (PORTB & ~_BV(P_BDPWR10)) | _BV(P_BDPWR200);
}

void diode_on_200uA(void) {
    PORTB &= ~(_BV(P_BDPWR10) | _BV(P_BDPWR200));
}

void diode_off(void) {
    PORTB |= _BV(P_BDPWR10) | _BV(P_BDPWR200);
}

static void enable_adc(void) {
    power_adc_enable();
    ADCSRA |= _BV(ADEN);
}

static void disable_adc(void) {
    ADCSRA &= ~_BV(ADEN);
    power_adc_disable();
}

static float adc() {
    const int N = 64;
    uint16_t result = 0;
    for (int i = 0; i < N; i++) {
        // start conversion
        ADCSRA |= _BV(ADSC);
        // wait for conversion to complete
        loop_until_bit_is_clear(ADCSRA, ADSC);
        //ADCL must be read first
        uint8_t result_low = ADCL;
        uint8_t result_high = ADCH;
        result += result_high << 8 | result_low;
    }
    return ((float)result) / N * 1.1f / 1024;
}

void init_thermometer(void) {
    //D+ and D- are input
    DDRB &= ~(_BV(P_DPLUS) | _BV(P_DMINUS));
    PORTB &= ~(_BV(P_DPLUS) | _BV(P_DMINUS));
    //BDPWR10 and BDPWR200 are output
    DDRB |= _BV(P_BDPWR10) | _BV(P_BDPWR200);

    diode_off();

    // use internal 1.1V voltage reference, no left adjust result, input channel is D+ - D-
    ADMUX = _BV(REFS1) | ADMUX_DIODE << MUX0;
    // ADC disabled, Auto Trigger disabled, ADC interrupt disabled, prescaler 1/2 (125kHz)
    ADCSRA = 0;
    // unipolar input mode, analog comparator multiplexer disabled, no input polarity reversal
    ADCSRB = 0;
    // disable digital input buffer of D+ and D-
    DIDR0 = _BV(DID_DPLUS) | _BV(DID_DMINUS);
    // analog comparator disabled
    ACSR = _BV(ACD);
    // shutdown adc and analog comparator
    power_adc_disable();
}

int8_t measure_temp(void) {
    enable_adc();

    // https://www.analog.com/media/en/technical-documentation/application-notes/an137f.pdf
    diode_on_10uA();
    float v10 = adc();
    diode_on_200uA();
    float v200 = adc();
    diode_off();

    disable_adc();

    float temp = (v10 - v200) / (IDEALITY_FACTOR * BOLTZMANN_CONSTANT / ELEMENTARY_CHARGE * logf(CURRENT_RATIO)) - 273.15;
    TRACE("THM: v10=%fmV v200=%fmV T=%fC\n", v10 * 1000, v200 * 1000, temp);

    return (int8_t)roundf(temp);
}

float read_diode_voltage(void) {
    enable_adc();
    float ret = adc();
    disable_adc();
    return ret;
}
