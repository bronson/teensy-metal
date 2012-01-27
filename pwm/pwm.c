#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include "utils.h"
#include "print.h"




int main(void)
{
    CPU_PRESCALE(CPU_16MHz);
    usb_init();

    DDRB |= 1<<5;
    DDRB |= 1<<6;

    PORTB |= 1<<5;

    while (1) {
        PORTB ^= (1<<5) | (1<<6);
        _delay_ms(250);
    }
}
