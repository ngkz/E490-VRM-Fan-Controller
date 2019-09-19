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

#ifndef __UART_H_
#define __UART_H_

#include <stdint.h>

void init_uart();
void putch(char ch);
char getch();
uint8_t available_input();
void putln();
void putchln(char ch);
void putP(const char *str /* PROGMEM */);
void putPln(const char *str /* PROGMEM */);
void put(const char *str);
void putd(int32_t number);
void putu(uint32_t number);
void putuln(uint32_t number);
void putf(float number, uint8_t digits);
void promptP(char *dest, uint8_t len, const char *prompt /* PROGMEM */);

#endif // __UART_H_
