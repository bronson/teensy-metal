#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include "utils.h"
#include "print.h"

// If you have LEDs connected to B5 and B6, they will be flashed using the PWM.
// Notice how the cpu enters an infinite loop and yet the LEDs still flash.


// CLKPR determines CLKio: 1/1 - 1/256 (16MHz - 62.5KHz)
// For clock 1, TCCR1B can divide CLKio: 1/1 - 1/1024 (16MHz - 15.6KHz)
//      for a 10 bit cycle, a 16 MHz count becomes a 15625 Hz cycle
//      and a 15 KHz count becomes a 15Hz cycle.  You can see this with your own eyes.


int main(void)
{
    CPU_PRESCALE(CPU_16MHz);
    usb_init();

    // set up clock 1:
    //   TCCR1A.COM1A=3    active high pwm on 1A
    //   TCCR1A.COM1B=3    active high pwm on 1B
    //   TCCR1A.WGM0=3     10 bit phase correct pwm: 0-0x3FF
    //   TCCR1B.WGM2=0     WGM=3 so upper 2 bits are 0
    //   TCCR1B.CS=5       5=CLKio/1024/cycle or 15Hz
    TCCR1A = (3<<COM1A0) | (3<<COM1B0) | (3<<WGM10);
    TCCR1B = (0<<WGM12)  | (5<<CS10);

    DDRB = (1<<5) | (1<<6);   // CONFIGURE PB5 and PB6 for output, everything else for input

    OCR1A = 0x300;  // 1A is on for 3/4, off for 1/4
    OCR1B = 0x100;  // 1B is on for 1/4, off for 3/4

    while (1) {
        // do absolutely nothing
        _delay_ms(250);
    }
}

