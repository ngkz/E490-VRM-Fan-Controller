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

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "uart.h"
#include "asciichr.h"

#define BAUD_RATE               1200
#define P_TX                    PB0
#define P_RX                    PB1
#define PCINT_RX                PCINT1
#define SERIAL_BUFFER_SIZE      16
#define ONEBIT_DELAY_COUNT      ((F_CPU/BAUD_RATE-8)/3)

static char rxbuf [SERIAL_BUFFER_SIZE];
static volatile uint8_t rxbuf_head;
static uint8_t rxbuf_tail;

void putch(char ch) {
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

ISR(PCINT0_vect) {
    char ch;
    uint8_t dummy;
    asm volatile(
        "1:   ldi r25, %[startbitdelay]\n"           //^| Get to 0.25 of start bit (our baud is too fast, so give room to correct)
        "2:   dec r25\n"                             // | ONEBIT_DELAY_COUNT / 4 * 3 cycles
        "     brne 2b\n"                             //_|
        "3:   ldi r25, %[onebitdelay]\n"             //^|
        "4:   dec r25\n"                             // | ONEBIT_DELAY_COUNT * 3 cycles
        "     brne 4b\n"                             //_|
        "     clc\n"                                 //^|
        "     sbic %[pin], %[bit]\n"                // |
        "     sec\n"                                 // |
        "     dec  %[count]\n"                       // | 8 cycles
        "     breq 5f\n"                             // |
        "     ror  %[ch]\n"                          // |
        "     rjmp 3b\n"                             //_|
        "5:\n"
        : [ch] "=&r" (ch),
          "=r" (dummy)
        : [count] "1" ((uint8_t)9),
          [pin] "I" (_SFR_IO_ADDR(PINB)),
          [bit] "I" (P_RX),
          [onebitdelay] "M"(ONEBIT_DELAY_COUNT),
          [startbitdelay] "M"(ONEBIT_DELAY_COUNT/4)
        : "r25"
    );

    // store the received character into ring buffer
    uint8_t i = (rxbuf_head + 1) % SERIAL_BUFFER_SIZE;
    if (i != rxbuf_tail) {
        rxbuf[rxbuf_head] = ch;
        rxbuf_head = i;
    }

    // clear the interrupt flag
    GIFR |= _BV(PCIF);
}

char getch() {
    while (rxbuf_head == rxbuf_tail);

    char c = rxbuf[rxbuf_tail];
    rxbuf_tail = (rxbuf_tail + 1) % SERIAL_BUFFER_SIZE;

    return c;
}

uint8_t available_input() {
    return (SERIAL_BUFFER_SIZE + rxbuf_head - rxbuf_tail) % SERIAL_BUFFER_SIZE;
}

void init_uart() {
    //TX output, high
    PORTB |= _BV(P_TX);
    DDRB |= _BV(P_TX);
    //RX input, pull-up
    DDRB &= ~_BV(P_RX); //RX input
    PORTB |= _BV(P_RX); //pull-up RX

    // RX level change generates a pin chage interrupt
    PCMSK = _BV(PCINT_RX);
    GIMSK |= _BV(PCIE);
}

void putln() {
    putch('\r');
    putch('\n');
}

void putchln(char ch) {
    putch(ch);
    putln();
}

void putP(const char *str /* PROGMEM */) {
    for (;;) {
        char ch = pgm_read_byte(str++);
        if (!ch) break;
        putch(ch);
    }
}

void putPln(const char *str /* PROGMEM */) {
    putP(str);
    putln();
}

void put(const char *str) {
    while (*str) putch(*str++);
}

void putd(int32_t number) {
    if (number < 0) {
        putch('-');
        number = -number;
    }

    putu(number);
}

void putu(uint32_t number) {
    char buf[10 + 1];
    char *str = &buf[sizeof(buf) - 1];

    *str = '\0';

    do {
        *--str = '0' + number % 10;
        number /= 10;
    } while(number);

    put(str);
}

void putuln(uint32_t val) {
    putu(val);
    putln();
}

void putf(float number, uint8_t digits) {
    // Handle negative numbers
    if (number < 0.0) {
         putch('-');
         number = -number;
    }

    // Round correctly so that print(1.999, 2) prints as "2.00"
    float rounding = 0.5;
    for (uint8_t i=0; i<digits; ++i) {
        rounding /= 10.0;
    }

    number += rounding;

    // Extract the integer part of the number and print it
    unsigned long int_part = (unsigned long)number;
    float remainder = number - (float)int_part;
    putu(int_part);

    // Print the decimal point, but only if there are digits beyond
    if (digits > 0) {
        putch('.');
    }

    // Extract digits from the remainder one at a time
    while (digits-- > 0) {
        remainder *= 10.0;
        int toPrint = (int)remainder;
        putu(toPrint);
        remainder -= toPrint;
    }
}

void promptP(char *dest, uint8_t len, const char *prompt /* PROGMEM */) {
    putP(prompt);

    size_t idx = 0;

    for (;;) {
        char ch = getch();
        switch(ch) {
        case ASCII_CR:
        case ASCII_LF:
            if (len > 0) dest[idx] = 0;
            putln();
            return;
        case ASCII_BS:
        case ASCII_DEL:
            if (idx > 0) {
                idx--;
                putch(ASCII_BS);
                putch(' ');
                putch(ASCII_BS);
            }
            break;
        default:
            if (idx + 1 < len) {
                dest[idx] = ch;
                idx++;
                putch(ch);
            } else {
                putch(ASCII_BEL);
            }
            break;
        }
    }
}
