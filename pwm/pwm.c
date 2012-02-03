#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include "avrpwm.h"
#include "utils.h"

// If you have LEDs connected to B5 and B6, they will be flashed using the 16 bit PWM.
// This demonstrates both simple levels and continuous ramps.



// CLKPR determines CLKio: 1/1 - 1/256 (16MHz - 62.5KHz)
// For clock 1, TCCR1B can divide CLKio: 1/1 - 1/1024 (16MHz - 15.6KHz)
//      for a 10 bit cycle, a 16 MHz count becomes a 15625 Hz cycle
//      and a 15 KHz count becomes a 15Hz cycle.  You can see this with your own eyes.

void flash_leds(int max)
{
    // 0=off, max=maximum brightness
    while(1) {
        OCR1A = max/2; OCR1B = max/8;
        _delay_ms(500);
        OCR1A = max;   OCR1B = max/2;
        _delay_ms(500);
        OCR1A = max/2; OCR1B = max;
        _delay_ms(500);
        OCR1A = max/8; OCR1B = max/2;
        _delay_ms(500);
    }
}


void breathe_leds(int max)
{
    int i=0, j=0;              // current value of each led
    int idelta=4, jdelta=14;   // rate of change of the led (leds are incremented 100 times a second, range is 0..max)
    char idown=0, jdown=0;     // tells if count is going up or down (getting brighter or dimmer)

    while (1) {
        // increment i and turn around if we're past an extreme
        if(idown) {
            i -= idelta;
            if(i<0) {
                i = -i ;
                idown = 0;
            }
        } else {
            i += idelta;
            if(i > max) {
                i = max;   // would be more correct to say i=2*max-i but nobody will notice
                idown = 1;
            }
        }

        if(jdown) {
            j -= jdelta;
            if(j < 0) {
                j = -j;
                jdown = 0;
            }
        } else {
            j += jdelta;
            if(j > max) {
                j = max;
                jdown = 1;
            }
        }

        // set the pwm values
        OCR1A = i;
        OCR1B = j;
        _delay_ms(10);
    }
}


int main(void) __attribute__((noreturn));
int main(void)
{
#if F_CPU == 16000000UL
    CPU_PRESCALE(CPU_16MHz);
#else
#error prescaler set up wrong
#endif

    unsigned int clock1_max = 0x3FF;
    clock1_init(
            CS_clkio,        // don't prescale, run straight off CLKio
            WGM1_phase_correct_pwm_to_3FF,
            COM_pwm_normal,  // active high pwm on OCR1A
            COM_pwm_normal   // active high pwm on OCR1B
            );

    DDRB = (1<<5) | (1<<6);   // CONFIGURE PB5 and PB6 for output, everything else is input

    // change the if statement to select the demo
    if(0) {
        flash_leds(clock1_max);
    } else {
        breathe_leds(clock1_max);
    }

    while(1) { }
}

