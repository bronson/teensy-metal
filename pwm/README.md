## Watcher

Uses PWM to output "breathing" lights on pins B5 and B6.


The Teensy++ 2.0 has two wm modes: fast and phase-correct.  Fast just uses a sawtooth wave, counting up from 0 to TOP
For 8-bit timer, TOP is either FF or OCRA depending on setting of WGM
> All occurrences of OCRA in the docs are wrong?  Is that supposed to be OCR0A?


Available output compare pins on the Teensy++ 2.0:
- OC0A OC0B -- works with  8-bit timer/counter 0 (8 bit prescaler or T0 pin)
- OC1A OC1B OC1C -- works with 16 bit timer/counter 1 (prescaler or T1 pin)  also ICP1 to use external trigger to capture clock value
- OC2A OC2B -- works with  8 bit timer/counter 2 (8 bit prescaler or TOSC1 or TOSC1/TOSC2, async)
- OC3A OC3B OC3C -- works with 16 bit timer/counter 3, same as OC1?   also ICP3

The rogues gallery of 8-bit timer0 registers:
{{{
  TCNT0 -- current timer value.  controlled using TCCR0A and TCCR0B
    writing to TCNT0 clears compare match.
  OC0A OC0B etc. -- output compare output pins
    set up OC0x before setting DDR for the pin output
  OCR0A OCR0B -- output compare register, continuously compared to TCNT0
      match generates an output compare interrupt or a waveform on the OC0x pin
      double buffered, syncd to top or bottom of range, so no output glitches (but normal and clear timer on compare don't double buffer)
  OCF0A OCF0B -- output comare flag, high if registers matched, write 1 to clear
  FOC0A FOC0B -- force output compare
  TCCR0A -- timer/counter control register
       WGM00 and WGM01 bits (Waveform Generation Mode) in TCCR0A, WGM02 in TCCR0B.
        0=normal, 1=PCPWM top=FF, 2=CTC, 3=FastPWM top=FF, 5=PCPWM top=OCRA, 7=FastPWM top=OCRA
       TCCR0A = _BV(COM0A0) will toggle OC0A pin on match
       COM0A 0:1: controls how OC0A pin responds.  0=disconnected, 1=toggle, 2=clear, 3=set
         non-pwm: 1=toggle OC0A on compare match, 2=clear on compare match, 3=set on compare match
         fast-pwm: 1=if WGM02=0 then disconnected else WGM02=1 toggle OC0A, 2 clear OC0A on match and set at TOP, 3 set OC0A on match and clear at TOP
         pcpwm: 1=if WGM02=0 then disconnected else WGM02=1 toggle on match, 2=clear on match when upcounting, set when down, 3=set on match when upcounting, clear when down
       COM0B: same as COM0A except toggle (=1) isn't supported
  TCCR0B -- timer/counter control register
    FOC0A FOC0B -- writing 1 forces output compare match on WG unit
    WGM02 -- see TCCR0A:WGM00:1
    CS02:0 -- 0=stopped, 1=clkIO, 2=clkIO/8, 3=clkIO/64, 4=clkIO/256, 5=clkIO/1024, 6=falling edge on T0 pin, 7=rising edge on T0

  TIMSK0 -- timer interrupt mask, enables OCIE0A OCIE0B output compare match A and B interrupts, and TOIE0 overflow interrupt
  TIFR0 -- timer 0 interrupt flag register: flags indicating matches happened, clear by writing 1
  TOV0 -- Timer/Counter Overflow Flag, set when counter reaches bottom (only PCPWM?)

Timer modes:
  WGM0 2:0 = 0: normal: counting direction is up until TOP, starts over at 0, no counter clear.  no double buffering, write counter value at any time.
  WGM0 2:0 = 2: clear timer on compare: counter is reset to 0 when it matches OCR0A.  Therefore OCR0A defines the top.
  WGM0 2:0 = 3 or 7: fast PWM: single slope, resets to 0.  counter increases until it matches TOP, then cleared at next clock.
    Output is fast but can be glitchy when setting new values
    COM0x 1:0: 2 produces a non-inverted PWM, 3 inverts PWM, 1 toggles OC0A if compare natches AND WGM02 is set.
    If OCR0A is 0, output is a narrow spike (! no zero?), if it's MAX then the output is constantly high.
    Fast PWM Freq: fclkIO / (N*256) where N is the prescale factor: 1, 8, 64, 256, 1024
  WGM0 2:0 = 1 or 5: phase correct PWM: dual slope.  TOP is 0xFF if WGM2:0=1, or OCR0A if WGM2:0=5.
    Lower maximum frequency but symmetric. Better for motors.
    OC0x is cleared on compare match between TCNT0 and OCR0x when upcounting, and set if matched while downcounting.
    Phase PWM Freq: fclkIO / (N*510) where N is prescale factor
    PCPWM can be solid low or high, it doesn't have the glitchy low of the fast PWM

If TCNT0 equals OCR0A or OCR0B then the corresponding output compare flag OCF0A or OCF0B is set at the next timer clock.
The OCF can be reset by writing a 1 to its bit location.
If the corresponding interrupt is enabled, an output compare interrupt will be triggered (the OCF is cleared (before?) the
ISR runs).

  The word's most confusing name, force output compare FOC0A or FOC0B can be used to force the output comparator to trigger
  (doesn't change the timer or set the OCF0x flag, but apparently the OC0x pin goes high)

ISR(TIMER0_COMPA_vect) { ... }  If TIMSK0 = 2, we'll interrupt when timer0 (TCNT0?) == OCR0A


16-bit timer1 registers:    n is timer number (1 or 3), x is the output compare channel (a or b)
  TCNT1H, TCNT1L, TCNT3H, TCNT3L -- the counter
    write the high byte first. it gets stored in a shared temp reg and written all at once.  read low byte first.
    You can access the entire register in C and the compiler will do the right thing: TCNT1 = 0x1FF;
    If an interrupt modifies a 16 bit register, the main code must disable interrupts when performing 16-bit accesses.
  TCCR1A TCCR3A
    COM1A COM1B COM1C COM3A COM3B COM3C -- 0=normal, 1=CTC (toggle), 2=clear OCnx on match, 3=set on match
    WGM1 WGM3 -- waveform generation, table 14-4: normal, fast or pc pwm (8,9,10 bit), pc&fc pwm
  TCCR1B TCCR3B
    ICNC1 ICN3 -- input capture noise canceller
    ICES1 ICES3 -- input capture edge select
    WGM1 WGM3 -- see TCCR1A/TCCR3A
    CS1, CS3 -- clock select: 0=stopped, 1=CLKio/1, 2=/8 3=/64 4=/256 5=/1024 6=clock on Tn pin, falling edge, 7=rising edge
  TCCR1C TCCR3C
    FOC1A FOC1B FOC1C FOC3A FOC3B FOC3C -- force output compare match
  OCR1A(HL), OCR1B(HL), OCR1C(HL), OCR3A(HL), OCR3B(HL), OCR3C(HL) -- output compare register
  ICR1H ICR1L ICR3H ICR3L -- input capture register
  TIMSK1 -- timer/counter 1 interrupt mask register
    ICIE1 ICIE3 -- input capture interrupt enable
    OCIE1A OCIE1B OCIE1C OCIE3A OCIE3B OCIE3C -- output compare interrupt enables
  TIFR1 TIFR3 -- timer interrupt flag register
    ICF1 ICF3: set when input capture fires
    OCF1A OCF1B OCF1C OCF3A OCF3B OCF3C: output compare fires
    TOV1 TOV3: timer overflow interrupt fired.  See table 14-4 for behavior.
  PRTIM1, PRTIM3 -- power reduction timer, write one to disable or zero to enable the corresponding timer.  they come powered up.
  OCF1C OCF1B OCF1A -


8-bit timer2 registers:
  The big difference between this and timer0 is the option of using a crystal to get much more accurate timing.
  First, PRTIM2 in PRR0 must be 0 so this timer is powered.
  TCNT2 -- the counter
  TCCR2A timer/counter control register
    COM2A COM2B -- tells if output pin should be ignored, toggled, set, or cleared on match, or inverted if in PWM mode.
    WGM20 WGM21 -- configures the CTC or PWM settings
  TCCR2B timer/counter control register
    FOC2A FOC2B -- force output compare; pretend that a compare was triggered
    WGM22 -- upper bit of WGM2
  OCR2A OCR2B -- output compare register, match affects output pin depending on setting of COM2A COM2B.
  ASSR -- this is the only meaningful difference between this and timer0
    EXCLK -- if 1 then external clock is on TOSC1 pin instead of using a crystal.
    AS2 -- asynchronous timer/counter 2: 0 means use CLKio, 1 means use external clock.
      Writing can corrupt some registers so write AS2 before TCNT2, OCR2A, OCR2B, TCCR2A and TCCR2B
    TCN2UB -- Timer/Counter 2 Update Busy: when operating async, indicates a write to TCNT2 is pending so don't write to it
      If you do, you might corrupt the value being set and/or cause a spurious interrupt.
    OCR2AUB OCR2BUB -- Output Compare Register 2 Busy: when operating async, indicates a write to OCR2A OCR2B is pending.
    TCR2AUB TCR2BUB -- Timer/Counter Control Register 2 Update Busy: when operating async, a write to TCCR2A TCCR2B is pending.
  TIMSK2 -- timer/counter 1 interrupt mask register
    TOIE2 -- enable Timer/Counter 2 Overflow Interrupt
    OCIE2A OCIE2B -- enable Output Compare interrupts (fire when OCR2A OCR2B matches TCNT2)
  TIFR2 -- timer interrupt flag register
    OCF2A OCF2B: Output Compare Flag: gets set to 1 when a compare match occurs between TCNT2 and OCR2A OCR2B.
    TOV2: Timer Overflow Flag: timer overflow interrupt has fired.
  GTCCR -- General Timer/Counter Control Register
    TSM -- Timer/Counter Synchronization Mode: keeps timers halted until cleared so clocks can be synchronized.
    PSRASY -- Prescaler Reset Timer/Counter 2: resets timer2, remains 1 until the prescaler has reset.  Not cleared if TSM is set.
    PSRSYNC -- if 1, all prescalers are being reset.

}}}

