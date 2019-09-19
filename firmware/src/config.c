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
#include "uart.h"
#include "thermometer.h"
#include "fancontrol.h"

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

struct Command {
    const char *name_P;
    char need_argument;
    void (*impl)(const char *arg);
};

static void show_information(const char *arg);
static void calibrate_thermometer(const char *arg);
static void set_min_duty(const char *arg);
static void set_max_duty(const char *arg);
static void set_startup_duty(const char *arg);
static void set_fan_start_temp(const char *arg);
static void set_fan_full_speed_temp(const char *arg);
static void set_fan_stop_temp(const char *arg);
static void set_pulse_per_revolution(const char *arg);
static void set_min_rpm(const char *arg);
static void set_fg_delay(const char *arg);
static void toggle_trace(const char *arg);
static void show_help(const char *arg);

//avr-gcc doesn't merge duplicated PSTR constant :(
static const char TEMPERATURE_PROMPT[] PROGMEM = "Input temperature: ";
static const char SPACE_PAREN[] PROGMEM = " (";

static const char INFO[] PROGMEM = "info";
static const char CALIBRATE[] PROGMEM = "calibrate";
static const char MIN_DUTY[] PROGMEM = "min_duty";
static const char MAX_DUTY[] PROGMEM = "max_duty";
static const char STARTUP_DUTY[] PROGMEM = "startup_duty";
static const char FAN_START_TEMP[] PROGMEM = "fan_start_temp";
static const char FAN_FULL_SPEED_TEMP[] PROGMEM = "fan_full_speed_temp";
static const char FAN_STOP_TEMP[] PROGMEM = "fan_stop_temp";
static const char PULSE_PER_REVOLUTION[] PROGMEM = "pulse_per_revolution";
static const char MIN_RPM[] PROGMEM = "min_rpm";
static const char FG_DELAY[] PROGMEM = "fg_delay";
static const char TRACE[] PROGMEM = "trace";
static const char HELP[] PROGMEM = "help";
static const char EXIT[] PROGMEM = "exit";

static const struct Command commands[] PROGMEM = {
    {INFO,                    0, show_information},
    {CALIBRATE,               0, calibrate_thermometer},
    {MIN_DUTY,                1, set_min_duty},
    {MAX_DUTY,                1, set_max_duty},
    {STARTUP_DUTY,            1, set_startup_duty},
    {FAN_START_TEMP,          1, set_fan_start_temp},
    {FAN_FULL_SPEED_TEMP,     1, set_fan_full_speed_temp},
    {FAN_STOP_TEMP,           1, set_fan_stop_temp},
    {PULSE_PER_REVOLUTION,    1, set_pulse_per_revolution},
    {MIN_RPM,                 1, set_min_rpm},
    {FG_DELAY,                1, set_fg_delay},
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
    putu(config.zero_c_voltage);
    putP(SPACE_PAREN);
    putu(ADC_VALUE_TO_VOLTAGE(config.zero_c_voltage));
    putPln(PSTR("mV)"));

    putP(PSTR("Temperature coefficient: "));
    putf(config.temperature_coefficient, 3);
    putP(SPACE_PAREN);
    putf(ADC_VALUE_TO_VOLTAGE(config.temperature_coefficient), 3);
    putPln(PSTR("mV/C)"));
}

static void show_information(const char *arg) {
    putP(PSTR("Temp: ")); putd(measure_temp()); putchln('C');
    putPln(PSTR("---"));
    show_thermometer_config();
    putP(PSTR("Min duty: ")); putuln(config.min_duty);
    putP(PSTR("Max duty: ")); putuln(config.max_duty);
    putP(PSTR("Startup duty: ")); putuln(config.startup_duty);
    putP(PSTR("Fan start temp: ")); putd(config.fan_start_temp); putchln('C');
    putP(PSTR("Fan full speed temp: ")); putd(config.fan_full_speed_temp); putchln('C');
    putP(PSTR("Fan stop temp: ")); putd(config.fan_stop_temp); putchln('C');
    putP(PSTR("Pulse per revolution: ")); putuln(config.pulse_per_revolution);
    putP(PSTR("Min RPM: ")); putu(config.min_rpm); putPln(PSTR("rpm"));
    putP(PSTR("FG Delay: ")); putu(config.fg_delay); putPln(PSTR("ms"));
}

static void put_voltage_temperature(uint16_t voltage, float temperature) {
    putu(voltage);
    putP(SPACE_PAREN);
    putu(ADC_VALUE_TO_VOLTAGE(voltage));
    putP(PSTR("mV) @ "));
    putf(temperature, 1);
    putchln('C');
}

static void calibrate_thermometer(const char *arg) {
    char buf[7];

    putPln(PSTR("Cool the diode as much as possible."));
    promptP(buf, sizeof(buf), TEMPERATURE_PROMPT);
    float    temperature_low = (float)atof(buf);
    uint16_t voltage_low = adc_diode_voltage();

    put_voltage_temperature(voltage_low, temperature_low);

    putPln(PSTR("Heat the diode as much as possible."));
    promptP(buf, sizeof(buf), TEMPERATURE_PROMPT);
    float    temperature_high = (float)atof(buf);
    uint16_t voltage_high = adc_diode_voltage();

    put_voltage_temperature(voltage_high, temperature_high);

    float    temperature_coefficient = (voltage_high - voltage_low) / (temperature_high - temperature_low);
    uint16_t zero_c_voltage = voltage_low - (temperature_low - 0) / temperature_coefficient;

    config.temperature_coefficient = temperature_coefficient;
    config.zero_c_voltage = zero_c_voltage;
    save_config();

    show_thermometer_config();
}

static void set_uint8(uint8_t *dest, const char *arg) {
    *dest = atoi(arg);
    save_config();
}

static void set_uint16(uint16_t *dest, const char *arg) {
    *dest = atoi(arg);
    save_config();
}

static void set_min_duty(const char *arg) {
    set_uint8(&config.min_duty, arg);
}

static void set_max_duty(const char *arg) {
    set_uint8(&config.max_duty, arg);
}

static void set_startup_duty(const char *arg) {
    set_uint8(&config.startup_duty, arg);
}

static void set_fan_start_temp(const char *arg) {
    set_uint8((uint8_t *)&config.fan_start_temp, arg);
}

static void set_fan_full_speed_temp(const char *arg) {
    set_uint8((uint8_t *)&config.fan_full_speed_temp, arg);
}

static void set_fan_stop_temp(const char *arg) {
    set_uint8((uint8_t *)&config.fan_stop_temp, arg);
}

static void set_pulse_per_revolution(const char *arg) {
    set_uint8(&config.pulse_per_revolution, arg);
}

static void set_min_rpm(const char *arg) {
    set_uint16(&config.min_rpm, arg);
}

static void set_fg_delay(const char *arg) {
    set_uint16(&config.fg_delay, arg);
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
