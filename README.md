# ThinkPad E490 VRM Cooling Mod

- WIP
- Only for IGP model
- Maybe this mod also applicable to E590, E480, and E580

## How to flash the microcontroller
### Set fuse bits

``` sh
$ pio run -t fuses
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

``` sh
$ vi firmware/src/main.c
#define TUNED_OSCCAL     <CALIBRATED OSCCAL HERE>
```

### Upload firmware

``` sh
$ pio run -t upload
```
