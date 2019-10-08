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
#define P_TX_RX                 PB2
#define PCINT_TX_RX             PCINT0
#define SERIAL_BUFFER_SIZE      16
#define ONEBIT_DELAY_COUNT      (F_CPU/BAUD_RATE)

static int uart_putchar(char c, FILE *stream);
static int uart_getchar(FILE *stream);

static FILE uart_stdio = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);
static char rxbuf [SERIAL_BUFFER_SIZE];
static volatile uint8_t rxbuf_head;
static uint8_t rxbuf_tail;

static void putch(char ch) {
    uint8_t oldSREG = SREG;
    cli(); //Prevent interrupts from breaking the transmission.
    //it can either receive or send, not both (because receiving requires an interrupt and would stall transmission
    uint8_t dummy1, dummy2;
    asm volatile(
        "   com %[ch]\n" // ones complement, carry set
        "1: brcc 2f\n"                     //^|
        "   cbi %[port],%[bit]\n"          // |
        "   sbi %[ddr],%[bit]\n"           // |
        "   rjmp 3f\n"                     // | 5 cycles
        "2: cbi %[ddr],%[bit]\n"           // |
        "   sbi %[port],%[bit]\n"          // |
        "   nop\n"                         //_|
        "3: ldi r25, %[delaycount]\n "     //^|
        "4: dec r25\n"                     // | ONEBIT_DELAY_COUNT cycles
        "   brne 4b\n"                     // |
#if (ONEBIT_DELAY_COUNT - 9) % 3 >= 1      // |
        "   nop\n"                         // |
#endif                                     // |
#if (ONEBIT_DELAY_COUNT - 9) % 3 >= 2      // |
        "   nop\n"                         // |
#endif                                     //_|
        "   lsr %[ch]\n"                   //^|
        "   dec %[bitcount]\n"             // | 4 cycles
        "   brne 1b\n"                     //_|
        : "=r" (dummy1),
          "=r" (dummy2)
        : [ch] "0" (ch),
          [bitcount] "1" ((uint8_t)10),
          [port] "I" (_SFR_IO_ADDR(PORTB)),
          [ddr] "I" (_SFR_IO_ADDR(DDRB)),
          [bit] "I" (P_TX_RX),
          [delaycount] "M"((ONEBIT_DELAY_COUNT-9) / 3)
        : "r25"
    );
    GIFR |= _BV(PCIF);
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
        "4:   dec r25\n"                             // | ONEBIT_DELAY_COUNT cycles
        "     brne 4b\n"                             // |
#if (ONEBIT_DELAY_COUNT - 8) % 3 >= 1                // |
        "   nop\n"                                   // |
#endif                                               // |
#if (ONEBIT_DELAY_COUNT - 8) % 3 >= 2                // |
        "   nop\n"                                   // |
#endif                                               //_|
        "     clc\n"                                 //^|
        "     sbic %[pin], %[bit]\n"                 // |
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
          [bit] "I" (P_TX_RX),
          [onebitdelay] "M"((ONEBIT_DELAY_COUNT - 8) / 3),
          [startbitdelay] "M"(ONEBIT_DELAY_COUNT / 3 / 4)
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

static char getch(void) {
    while (rxbuf_head == rxbuf_tail);

    char c = rxbuf[rxbuf_tail];
    rxbuf_tail = (rxbuf_tail + 1) % SERIAL_BUFFER_SIZE;

    return c;
}

static int uart_getchar(FILE *stream) {
    return getch();
}

static int uart_putchar(char c, FILE *stream) {
    if (c == '\n') putch('\r');
    putch(c);
    return 0;
}

void init_uart(void) {
    //TX/RX is pulled-up input
    DDRB &= ~_BV(P_TX_RX);
    PORTB |= _BV(P_TX_RX);

    //generate a pin change interrupt when TX/RX changes
    PCMSK |= _BV(PCINT_TX_RX);
    GIMSK |= _BV(PCIE);

    stdout = &uart_stdio;
    stdin = &uart_stdio;
}

uint8_t available_input(void) {
    return (SERIAL_BUFFER_SIZE + rxbuf_head - rxbuf_tail) % SERIAL_BUFFER_SIZE;
}
