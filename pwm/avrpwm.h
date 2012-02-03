// Definitions to make programming the 1286's PWMs more readable


// if you want to run the PWM at a different CLKio than the CPU,
// define F_PWM before #including this file.
#ifndef F_PWM
#define F_PWM F_CPU
#endif


#if defined(__AVR_AT90USB1286__)

typedef enum {
    CS_stopped = 0,
    CS_clkio = 1,
    CS_external_clock_on_falling_edge = 6,
    CS_external_clock_on_rising_edge = 7,

#if F_PWM == 16000000UL
// Tells how long each count will take.  Time for a full cycle (Hz) depends on TOP value.
    CS_16MHz_count = 1,
    CS_2MHz_count = 2,
    CS_250KHz_count = 3,
    CS_62500Hz_count = 4,
    CS_15265Hz_count = 5
#else
#error "Unknown clock"
#endif
} ClockSelect;


typedef enum {
    WGM1_normal                     = 0,
    WGM1_phase_correct_pwm_to_FF    = 1,
    WGM1_phase_correct_pwm_to_1FF   = 2,
    WGM1_phase_correct_pwm_to_3FF   = 3,
    WGM1_clear_timer_on_compare_match_OCR = 4,
    WGM1_fast_pwm_to_FF             = 5,
    WGM1_fast_pwm_to_1FF            = 6,
    WGM1_fast_pwm_to_3FF            = 7,
    // todo
    WGM1_clear_timer_on_compare_match_ICR = 12,
    WGM1_fast_pwm_to_ICR            = 14,
    WGM1_fast_pwm_to_OCR            = 15
} WaveformGenerationMode_Clock1;


typedef enum {
    WGM2_normal                     = 0,
    WGM2_phase_correct_pwm_to_FF    = 1,
    WGM2_clear_timer_on_compare_match=2,
    WGM2_fast_pwm_to_FF             = 3,
    WGM2_phase_correct_pwm_to_OCRA  = 5,
    WGM2_fast_pwm_to_OCRA           = 7
} WaveformGenerationMode_Clock2;


typedef enum {
    COM_disconnected          = 0,
    COM_toggled_on_match      = 1,
    COM_cleared_on_match      = 2,
    COM_set_on_match          = 3,

    COM_pwm_inverted          = 2,
    COM_pwm_normal            = 3
} CompareOutputMode;


//#define set_timer0(cs, wgm, com0a, com0b) do { \
      //#error "no idea"
    //} while(0)


static inline void clock1_init(ClockSelect cs, WaveformGenerationMode_Clock1 wgm, CompareOutputMode com1a, CompareOutputMode com1b)
{
    TCCR1A = ((com1a)<<COM1A0) | ((com1b)<<COM1B0) | (((wgm)&3)<<WGM10);
    TCCR1B = (((wgm)&4?1:0)<<WGM12)  | (cs<<CS10);
}


#else
#error "Board not supported yet"
#endif

