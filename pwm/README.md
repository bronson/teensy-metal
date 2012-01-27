## Watcher

Uses PWM to output "breathing" lights on pins B5 and B6.


Timer0: 8 bit (8 bit prescaler or T0 pin)
Timer1: 16 bit (prescaler or T1 pin)
Timer2: 8 bit (10 bit prescaler or TOSC1 or TOSC1/TOSC2 (crystal) pins, asynchronous operation)
      (async means that outputs can appear independent of being clocked by MCU)
Timer3: 16 bit (prescaler only?)


The Teensy++ 2.0 has two pwm modes: fast and phase-correct.  Fast just uses a sawtooth wave, counting up from 0 to TOP
For 8-bit timer, TOP is either FF or OCRA depending on setting of WGM
  OCRA is a bug right?  Is that supposed to be OCR0A?


Available OC pins on the Teensy++ 2.0:
  OC0A OC0B       -- timer/counter 0
  OC1A OC1B OC1C  -- 
  OC2A OC2B
  OC3A OC3B OC3C

The rogues gallery of 8-bit timer registers:
  TCNT0 -- current timer value.  controlled using TCCR0A and TCCR0B
    writing to TCNT0 clears compare match.
  OC0A OC0B OC1A etc. -- output compare output pins
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


16-bit timer registers:
  OCF1C OCF1B OCF1A -

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
