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

/*
  Part of this code derived from ATTinyCore:
  TinySoftwareSerial.cpp - Hardware serial library for Wiring
  Copyright (c) 2006 Nicholas Zambetti.  All right reserved.
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
  Modified 23 November 2006 by David A. Mellis
  Modified 28 September 2010 by Mark Sproul

  Print.cpp - Base class that provides print() and println()
  Copyright (c) 2008 David A. Mellis.  All right reserved.
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
  Modified 23 November 2006 by David A. Mellis
 */

#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "uart.h"

#define BAUD_RATE               4800
#define P_TX                    PB5
#define ONEBIT_DELAY_COUNT      ((F_CPU/BAUD_RATE-8)/3)

static int uart_putchar(char c, FILE *stream);

static FILE uart_stdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

static void putch(char ch) {
    uint8_t oldSREG = SREG;
    cli(); //Prevent interrupts from breaking the transmission.
    //it can either receive or send, not both (because receiving requires an interrupt and would stall transmission
    uint8_t dummy1, dummy2;
    asm volatile(
        "   com %[ch]\n" // ones complement, carry set
        "1: brcc 2f\n"                     //^|
        "   cbi %[port],%[bit]\n"          // |
        "   rjmp 3f\n"                     // | 4 cycles
        "2: sbi %[port],%[bit]\n"          // |
        "   nop\n"                         //_|
        "3: ldi r25, %[delaycount]\n "     //^|
        "4: dec r25\n"                     // | ONEBIT_DELAY_COUNT * 3 cycles
        "   brne 4b\n"                     // |
        "   lsr %[ch]\n"                   //^|
        "   dec %[bitcount]\n"             // | 4 cycles
        "   brne 1b\n"                     //_|
        : "=r" (dummy1),
          "=r" (dummy2)
        : [ch] "0" (ch),
          [bitcount] "1" ((uint8_t)10),
          [port] "I" (_SFR_IO_ADDR(PORTB)),
          [bit] "I" (P_TX),
          [delaycount] "M"(ONEBIT_DELAY_COUNT)
        : "r25"
    );

    SREG = oldSREG;
    return;
}

static int uart_putchar(char c, FILE *stream) {
    putch(c);
    return 0;
}

void init_uart(void) {
    //TX output, high
    PORTB |= _BV(P_TX);
    DDRB |= _BV(P_TX);

    stdout = &uart_stdout;
}
