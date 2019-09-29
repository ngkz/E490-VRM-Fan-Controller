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
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include "uart.h"
#include "thermometer.h"
#include "fancontrol.h"

struct Config config_EE EEMEM = {
    .zero_c_voltage          = 572.571,
    .temperature_coefficient = -1.540,
};

struct Config config;

struct Command {
    const char *name_P;
    char need_argument;
    void (*impl)(const char *arg);
};

static void show_information(const char *arg);
static void calibrate_thermometer(const char *arg);
static void toggle_trace(const char *arg);
static void show_help(const char *arg);

//avr-gcc doesn't merge duplicated PSTR constant :(
static const char TEMPERATURE_PROMPT[] PROGMEM = "Input temperature: ";
static const char SPACE_PAREN[] PROGMEM = " (";
static const char CANCELLED[] PROGMEM = "cancelled";

static const char INFO[] PROGMEM = "info";
static const char CALIBRATE[] PROGMEM = "calibrate";
static const char TRACE[] PROGMEM = "trace";
static const char HELP[] PROGMEM = "help";
static const char EXIT[] PROGMEM = "exit";

static const struct Command commands[] PROGMEM = {
    {INFO,                    0, show_information},
    {CALIBRATE,               0, calibrate_thermometer},
    {TRACE,                   0, toggle_trace},
    {HELP,                    0, show_help},
    {EXIT,                    0, NULL},
};

#define N_COMMANDS (sizeof(commands) / sizeof(commands[0]))

static void load_config() {
    eeprom_busy_wait();
    eeprom_read_block(&config, &config_EE, sizeof(struct Config));
}

void init_config() {
    load_config();
}

static void save_config() {
    eeprom_busy_wait();
    eeprom_write_block(&config, &config_EE, sizeof(struct Config));
}

static void show_thermometer_config() {
    putP(PSTR("Voltage at 0C: "));
    putf(config.zero_c_voltage, 3);
    putP(SPACE_PAREN);
    putf(adc_value_to_mv(config.zero_c_voltage), 3);
    putPln(PSTR("mV)"));

    putP(PSTR("Temperature coefficient: "));
    putf(config.temperature_coefficient, 3);
    putP(SPACE_PAREN);
    putf(adc_value_to_mv(config.temperature_coefficient), 3);
    putPln(PSTR("mV/C)"));
}

static void show_information(const char *arg) {
    putP(PSTR("Temp: ")); putd(measure_temp()); putchln('C');
    putPln(PSTR("---"));
    show_thermometer_config();
}

static void put_voltage_temperature(int16_t voltage, float temperature) {
    putu(voltage);
    putP(SPACE_PAREN);
    putu(round(adc_value_to_mv(voltage)));
    putP(PSTR("mV) @ "));
    putf(temperature, 1);
    putchln('C');
}

static void calibrate_thermometer(const char *arg) {
    char buf[7];

    putPln(PSTR("Cool the diode as much as possible."));
    promptP(buf, sizeof(buf), TEMPERATURE_PROMPT);
    if (buf[0] == 0) {
        putPln(CANCELLED);
        return;
    }
    float temperature_low = (float)atof(buf);
    int16_t voltage_low = adc_diode_voltage();

    put_voltage_temperature(voltage_low, temperature_low);

    putPln(PSTR("Heat the diode as much as possible."));
    promptP(buf, sizeof(buf), TEMPERATURE_PROMPT);
    if (buf[0] == 0) {
        putPln(CANCELLED);
        return;
    }
    float temperature_high = (float)atof(buf);
    int16_t voltage_high = adc_diode_voltage();

    put_voltage_temperature(voltage_high, temperature_high);

    float temperature_coefficient = (voltage_high - voltage_low) / (temperature_high - temperature_low);
    float zero_c_voltage = voltage_low - temperature_coefficient * temperature_low;

    config.temperature_coefficient = temperature_coefficient;
    config.zero_c_voltage = zero_c_voltage;
    save_config();

    show_thermometer_config();
}

static void toggle_trace(const char *arg) {
    toggle_fan_control_trace();
}

static void show_help(const char *arg) {
    for (int i = 0; i < N_COMMANDS; i++) {
        struct Command cmd;
        memcpy_P(&cmd, &commands[i], sizeof(struct Command));

        putP(cmd.name_P);
        if (cmd.need_argument) {
            putch(' ');
            putP(PSTR("<ARG>"));
        }
        putln();
    }
}

void config_ui() {
    char line[24];

    for (;;) {
next:
        promptP(line, sizeof(line), PSTR("> "));

        if (line[0] == 0) continue;

        char *arg = strchr(line, ' ');
        if (arg) {
            *arg = 0;
            arg++;
        }
        char *cmd_name = line;

        for (int i = 0; i < N_COMMANDS; i++) {
            struct Command cmd;
            memcpy_P(&cmd, &commands[i], sizeof(struct Command));

            if (strcmp_P(cmd_name, cmd.name_P) == 0) {
                if (cmd.need_argument && !arg) goto err;
                if (!cmd.impl) return; //exit
                cmd.impl(arg);

                goto next;
            }
        }

err:
        show_help(NULL);
        continue;
    }
}
