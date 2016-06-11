#ifndef ARCH_X86_64
#  include <avr.h>
#  include <speaker.h>

#define DDR     DDRB
#define PORT    PORTB
#define PIN     3


void speaker_du()
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

static void speaker_set_period(uint16_t period)
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

void speaker_on()
{
        SET_BIT(DDR, PIN);
        sei();
}

void speaker_off()
{
        cli();
        CLR_BIT(DDR, PIN);
}

static void speaker_keep(uint16_t ms)
{
        TCCR0 = 2;
        while (ms--) {
                TCNT0 = 255;
                SET_BIT(TIFR, TOV0);
                while (!GET_BIT(TIFR, TOV0));
        }
        TCCR0 = 0;
}

void speaker_square_wave_software(uint16_t freq, uint16_t duration, float volume)
{
        speaker_on();

        uint16_t period = 2*1000000UL/freq/2;
        uint32_t n = (uint32_t) duration/(uint32_t) period * 500UL;
        uint16_t i;
        for (i = 0; i < n; i ++) {
                SET_BIT(PORT, PIN);
                speaker_keep(period);
                CLR_BIT(PORT, PIN);
                speaker_keep(period);
        }

        speaker_off();
}

void speaker_square_wave(uint16_t freq, uint16_t duration, float volume)
{
        speaker_on();

        uint16_t period = 1000000UL/freq/2;
        uint16_t dutycycle = 255*volume;
        OCR0 = dutycycle;

        speaker_set_period(period);
        speaker_keep(duration*1000);

        speaker_off();
}

#endif // ARCH_X86_64
