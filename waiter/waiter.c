#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include "utils.h"
#include "print.h"


int main(void)
{
    unsigned int i = 0;
    char buf[10];

    CPU_PRESCALE(CPU_16MHz);
    usb_init();

    // this should be unnecessary since it's configured this way on reset
    DDRD = 0;
    PORTD = 0;

    while (1) {
        itoa(i, buf, 10);
        pstr(buf);

        if(PIND & (1<<3)) {
            print(" D3 is HIGH\n");
        } else {
            print(" D3 is low\n");
        }

        i += 1;
        _delay_ms(250);
    }
}
