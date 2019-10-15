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
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include "thermometer.h"
#include "debug.h"

#define BOLTZMANN_CONSTANT 1.379553e-23f
#define ELEMENTARY_CHARGE  1.602177e-19f

#define P_THMPWR           PB3
#define P_THM              PB4    //ADC2
#define DID_THM            ADC2D
#define ADMUX_THM          0b0010 //ADC2
#define BETA               3408   //thermistor beta
#define R1                 3920
#define T0                 25     //[C]
#define R0                 9500   //thermistor resistance at T0[C]
#define ADC_GAIN_COMP      1.0026672f // 4.88V = 1022.5LSB (Vcc=4.88V)
#define ADC_OFFSET_ERROR   1.2239757f // 8.3mV = 1/2LSB (Vcc=4.93V)
#define ADC_PRESCALER      0b010

#if F_CPU != 1000000L
#error "update adc prescaler"
#endif

static void thermistor_on(void) {
    PORTB |= _BV(P_THMPWR);
}

static void thermistor_off(void) {
    PORTB &= ~_BV(P_THMPWR);
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
    const int N = 256;
    uint32_t sum = 0;

    set_sleep_mode(SLEEP_MODE_ADC);
    sleep_enable();

    for (int i = 0; i < N; i++) {
        // Enter ADC Noise Reduction mode. the ADC starts conversion. Wait for the conversion complete.
        do {
            sleep_cpu();
        } while(bit_is_set(ADCSRA, ADSC));
        // Read the result
        sum += ADC;
    }

    sleep_disable();

    return ((float)sum / N - ADC_OFFSET_ERROR) * ADC_GAIN_COMP;
}

void init_thermometer(void) {
    //THM is input
    DDRB &= ~_BV(P_THM);
    PORTB &= ~_BV(P_THM);
    //THMPWR is output
    DDRB |= _BV(P_THMPWR);
    thermistor_off();

    // Vref = Vcc, no left adjust result, input channel is THM
    ADMUX = ADMUX_THM << MUX0;
    // ADC disabled, Auto Trigger disabled, ADC interrupt enabled, prescaler 1/8 (125kHz)
    ADCSRA = _BV(ADIE) | ADC_PRESCALER << ADPS0;
    // unipolar input mode, analog comparator multiplexer disabled, no input polarity reversal
    ADCSRB = 0;
    // disable digital input buffer of THM
    DIDR0 = _BV(DID_THM);
    // analog comparator disabled
    ACSR = _BV(ACD);
    // shutdown adc and analog comparator
    power_adc_disable();
}

int8_t measure_temp(void) {
    enable_adc();

    thermistor_on();
    float v_thermistor_adc = adc();
    thermistor_off();

    disable_adc();

    float r_thermistor = R1 / (1024.0 / v_thermistor_adc /* Vcc/Vthm */ - 1);
    float temp  = 1.0 / (1.0 / (T0 + 273.15) + log(r_thermistor / R0) / BETA) - 273.15;
    TRACE("THM: THM=%fLSB R=%fΩ T=%f℃\n", v_thermistor_adc, r_thermistor, temp);

    return (int8_t)roundf(temp);
}

EMPTY_INTERRUPT(ADC_vect);
