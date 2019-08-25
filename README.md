# ThinkPad E490 VRM Cooling Mod

WIP

## How to flash the microcontroller
### Set fuse bits

``` sh
# internal 8MHz clock, Self-programming enabled
$ avrdude -c buspirate -P /dev/ttyUSB0 -p attiny85 -U lfuse:w:0xe2:m -U hfuse:w:0xdf:m -U efuse:w:0xfe:m
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
$ vi firmware/src/main.cpp
#define TUNED_OSCCAL     <CALIBRATED OSCCAL HERE>
```

### Patch and build bootloader

``` sh
$ git clone https://github.com/SpenceKonde/ATTinyCore
$ cd ATTinyCore
$ patch -p1 <<'EOS'
diff --git a/avr/bootloaders/optiboot/optiboot.c b/avr/bootloaders/optiboot/optiboot.c
index ecc21ca..2f84442 100644
--- a/avr/bootloaders/optiboot/optiboot.c
+++ b/avr/bootloaders/optiboot/optiboot.c
@@ -517,6 +517,7 @@ void pre_main(void) {
 
 /* main program starts here */
 int main(void) {
+  OSCCAL = <CALIBRATED OSCCAL HERE>;
   uint8_t ch;
 
   /*
@@ -833,7 +834,7 @@ int main(void) {
     rstVect0_sav = buff.bptr[rstVect0];
     rstVect1_sav = buff.bptr[rstVect1];
     addr16_t vect;
-    vect.word = ((uint16_t)main);
+    vect.word = ((uint16_t)main) - 1;
     buff.bptr[0] = vect.bytes[0]; // rjmp to start of bootloader
     buff.bptr[1] = vect.bytes[1] | 0xC0;  // make an "rjmp"
 #if (save_vect_num > SPM_PAGESIZE/2)
@@ -862,7 +863,7 @@ int main(void) {
         buff.bptr[saveVect0] = vect.bytes[0];
         buff.bptr[saveVect1] = (vect.bytes[1] & 0x0F)| 0xC0;  // make an "rjmp"
         // Add rjump to bootloader at RESET vector
-        vect.word = ((uint16_t)main); // (main) is always <= 0x0FFF; no masking needed.
+        vect.word = ((uint16_t)main) - 1; // (main) is always <= 0x0FFF; no masking needed.
         buff.bptr[0] = vect.bytes[0]; // rjmp 0x1c00 instruction
       }
EOS
$ cd avr/bootloaders/optiboot
$ make attiny85at8 PRODUCTION=1
$ avrdude -c buspirate -P /dev/ttyUSB0 -p attiny85 -U flash:w:optiboot_attiny85_8000000L.hex
```

### Upload firmware

``` sh
$ pio run -t upload
```
