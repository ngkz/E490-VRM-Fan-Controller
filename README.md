# ThinkPad E490 VRM Cooling Mod

- WIP
- Only for IGP model
- Maybe this mod also applicable to E590, E480, and E580

## How to flash the microcontroller
### Set fuse bits

``` sh
# Internal 8MHz clock, Self-programming enabled
avrdude -c buspirate -P/dev/ttyUSB0 -p attiny85 -U lfuse:w:0xE2:m -U hfuse:w:0xDF:m -U efuse:w:0xFE:m
```

### Calibrate internal oscillator

1. Build and flash the below program.
2. Connect TTL serial adapter RX to pin 5(TX), TX to pin 6(RX), a potentiometer to pin 2, and a frequency counter or oscilloscope to pin 3.
3. Calibrate output frequency to 1.00MHz with the potentiometer.
4. Read calibrated OSCCAL from TTL serial. (Baud rate is 19200bps)

``` c
#include <Arduino.h>

void setup() {
    Serial.begin(19200);

    pinMode(4, OUTPUT); //OC0B

    noInterrupts();
    TCCR1 = _BV(CTC1) | _BV(CS10) | _BV(COM1A1);
    GTCCR =  (GTCCR & ~(_BV(COM1B1) | _BV(COM1B0) | _BV(FOC1B) | _BV(FOC1A) | _BV(PSR1))) | _BV(PWM1B) | _BV(COM1B1);
    TCNT1 = 0;
    OCR1A = 0;
    OCR1B = 4;
    OCR1C = 7;
    TIMSK &= ~(_BV(OCIE1A) | _BV(OCIE1B) | _BV(TOIE1));
    TIFR |= _BV(OCF1A) | _BV(OCF1B) | _BV(TOV1);
    interrupts();
}

void loop() {
    OSCCAL = analogRead(A3) / 4;
    Serial.print("OSCCAL=");
    Serial.println(OSCCAL, DEC);
    delay(128);
}
```

```
$ editor firmware/src/main.c
#define TUNED_OSCCAL <CALIBRATED OSCCAL HERE>
```

```
$ editor optiboot/optiboot/bootloaders/optiboot/optiboot.c
#define TUNED_OSCCAL <CALIBRATED OSCCAL HERE>
```

### Install bootloader

```
$ cd optiboot/optiboot/bootloaders/optiboot
$ make attiny85at8 PRODUCTION=1
$ avrdude -c buspirate -P /dev/ttyUSB0 -p attiny85 -U flash:w:optiboot_attiny85_8000000L.hex
```

### Test bootloader
- Hook up LED and a resistor to PB0
- Check the LED blinks

```
$ cat <<'EOS' >/tmp/blink.c
#include <avr/io.h>
#include <util/delay.h>

int main() {
    DDRB = _BV(PB0);
    for (;;) {
        PORTB ^= _BV(PB0);
        _delay_ms(500);
    }
    return 0;
}
EOS
$ avr-gcc -o /tmp/blink.elf -Os -mmcu=attiny85 -DF_CPU=8000000L /tmp/blink.c
$ avrdude -c arduino -P /dev/ttyUSB0 -b 19200 -p attiny85 -U flash:w:/tmp/blink.elf
```

### Disable reset pin

```
# Internal 8MHz clock, Self-programming enabled, Reset disabled (Enable PB5 I/O pin)
$ avrdude -c buspirate -P/dev/ttyUSB0 -p attiny85 -U lfuse:w:0xE2:m -U hfuse:w:0x5F:m -U efuse:w:0xFE:m
```

### Upload firmware

```
$ pio run -t upload
```
