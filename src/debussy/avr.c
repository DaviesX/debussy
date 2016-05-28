#include <avr.h>


void avr_init()
{
        WDTCR = 15;
}

void avr_wait(uint16_t msec)
{
        TCCR0 = 3;
        while (msec--) {
                TCNT0 = (unsigned char)(256 - (XTAL_FRQ / 64) * 0.001);
                SET_BIT(TIFR, TOV0);
                while (!GET_BIT(TIFR, TOV0));
        }
        TCCR0 = 0;
}

void avr_wait2(uint16_t msec, uint16_t (*callback) (), void* data)
{
        TCCR0 = 3;
        while (msec-- > 0) {
                TCNT0 = (unsigned char)(256 - (XTAL_FRQ / 64) * 0.001);
                SET_BIT(TIFR, TOV0);
                while (!GET_BIT(TIFR, TOV0));
                uint16_t cost = callback(data);
                if (cost > msec) break;
                else msec -= cost;
        }
        TCCR0 = 0;
}

void avr_nop(uint16_t ms)
{
       volatile unsigned long i, n = (F_CPU/46000)*ms;
       for (i = 0; i < n; i ++) {
                continue;
       }
}

void avr_set_timer1_ctc(uint32_t milli)
{
        cli();                          // turn off global interrupt
        TCCR1A = 0;
        TCCR1B = 0;

        TCCR1B |= (1 << WGM12);
        // TCCR1B |= (1 << CS10);
        // TCCR1B |= (1 << CS11);
        TCCR1B |= (1 << CS12);

        // CS10 CS11 CS12
        // 0     0      1 -> 1/256 prescaling
        // f = F_CPU / 256
        OCR1A = (F_CPU/256 - 1)*milli/1000;

        TIMSK |= (1 << OCIE1A);
        sei();                          // enable global interrupt
}
