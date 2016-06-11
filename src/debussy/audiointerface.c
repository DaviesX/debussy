#ifndef ARCH_X86_64
#  include <avr.h>
#  include <audiointerface.h>

#define DDR     DDRB
#define PORT    PORTB
#define PIN     3


void audioif_du_avr()
{
        SET_BIT(DDR, PIN);
        uint16_t i;
        for (i = 0; i < 100; i ++) {
                SET_BIT(PORT, PIN);
                avr_wait(2);
                CLR_BIT(PORT, PIN);
                avr_wait(2);
        }
        CLR_BIT(DDR, PIN);
        CLR_BIT(PORT, PIN);
}

void audioif_on_avr()
{
        SET_BIT(DDR, PIN);
        sei();
}

void audioif_off_avr()
{
        cli();
        CLR_BIT(DDR, PIN);
}

static void audioif_keep(uint16_t ms)
{
        TCCR0 = 2;
        while (ms--) {
                TCNT0 = 255;
                SET_BIT(TIFR, TOV0);
                while (!GET_BIT(TIFR, TOV0));
        }
        TCCR0 = 0;
}

void audioif_output_1bit_sw_avr(uint16_t freq, uint16_t duration, float volume)
{
        uint16_t period = 2*1000000UL/freq/2;
        uint32_t n = (uint32_t) duration/(uint32_t) period * 500UL;
        uint16_t i;
        for (i = 0; i < n; i ++) {
                SET_BIT(PORT, PIN);
                audioif_keep(period);
                CLR_BIT(PORT, PIN);
                audioif_keep(period);
        }
}

static void __audioif_set_period_avr(uint16_t period)
{
        TCCR1A = 0;
        TCCR1B = 0;

        TCCR1B |= (1 << WGM12);
        TCCR1B |= (1 << CS11);

        OCR1A = period - 1;

        TIMSK |= (1 << OCIE1A);
}

static uint8_t flip = 1;

ISR(TIMER1_COMPA_vect)
{
        if (flip) SET_BIT(PORT, PIN);
        else CLR_BIT(PORT, PIN);
        flip = !flip;
}

void audioif_output_1bit_avr(uint16_t freq, uint16_t duration, float volume)
{
        uint16_t period = 1000000UL/freq/2;
        uint16_t dutycycle = 255*volume;
        OCR0 = dutycycle;

        __audioif_set_period_avr(period);
        audioif_keep(duration*1000);
}

void audioif_output_24bit_wave_avr(float fn)
{
}

#else
void audioif_on_x64()
{
}

void audioif_off_x64()
{
}

void audioif_output_24bit_wave_x64(float fn)
{
}
#endif // ARCH_X86_64
