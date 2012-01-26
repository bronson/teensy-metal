#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdlib.h>
#include "utils.h"
#include "usb_debug_only.h"
#include "print.h"


int main(void)
{
    int i = 0;
    char buf[10];

    CPU_PRESCALE(CPU_16MHz);
    usb_init();

    while (1) {
        itoa(i, buf, 10);
        pstr(buf);
        pchar('\n');
        i += 1;
        _delay_ms(12);
    }
}
