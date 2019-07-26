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

    for (;;) sleep_mode();
}
