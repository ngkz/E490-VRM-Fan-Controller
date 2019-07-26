#include <avr/io.h>
#include <avr/sleep.h>

#define PWM_N PB4
#define FG    PB3

FUSES = {
    .low = LFUSE_DEFAULT,
    .high = HFUSE_DEFAULT,
    .extended = EFUSE_DEFAULT
};

int main(void) {
    //Port configuration
    //Define directions for port pins
    DDRB = _BV(PWM_N);
    //Pull-up unconnected PB1 pin to avoid high power consumption.
    PORTB |= _BV(PB1);
    //Turn off the fan.
    PORTB |= _BV(PWM_N);

    //Disable unneeded modules to reduce power consumption
    //Analog comparator
    ACSR |= _BV(ACD);
    //Timer1, Timer0, ADC
    PRR = _BV(PRTIM1) | _BV(PRTIM0) | _BV(PRADC);

    //Stop CPU core until an interrupt occurs.
    for (;;) sleep_mode();
}
