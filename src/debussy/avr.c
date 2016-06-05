#include <avr.h>


void avr_init()
{
        wdt_enable(WDTO_2S);
}

void avr_wait(uint16_t milisec)
{
        // 1/64 prescale
        SET_BIT(TCCR0, CS00);
        SET_BIT(TCCR0, CS01);
        while (milisec --) {
                TCNT0 = (unsigned char)(256 - (XTAL_FRQ / 64) * 0.001);
                SET_BIT(TIFR, TOV0);
                while (!GET_BIT(TIFR, TOV0))
                        wdt_reset();
        }
        TCCR0 = 0;
}

void avr_wait2(uint16_t milisec, uint16_t (*callback) (), void* data)
{
        // 1/64 prescale
        SET_BIT(TCCR0, CS00);
        SET_BIT(TCCR0, CS01);
        while (milisec -- > 0) {
                TCNT0 = (unsigned char)(256 - (XTAL_FRQ / 64) * 0.001);
                SET_BIT(TIFR, TOV0);
                while (!GET_BIT(TIFR, TOV0))
                        wdt_reset();
                uint16_t cost = callback(data);
                if (cost > milisec) break;
                else milisec -= cost;
        }
        TCCR0 = 0;
}

void avr_wait_micro(uint16_t microsec)
{
        // 1/8 prescale
        SET_BIT(TCCR0, CS01);
        while (microsec --) {
                TCNT0 = (unsigned char)(256 - (XTAL_FRQ / 8) * 0.000001);
                SET_BIT(TIFR, TOV0);
                while (!GET_BIT(TIFR, TOV0))
                        wdt_reset();
        }
        TCCR0 = 0;
}

void avr_nop(uint16_t ms)
{
       volatile unsigned long i, n = (F_CPU/46000)*ms;
       for (i = 0; i < n; i ++) {
                wdt_reset();
       }
}

void avr_set_timer1_ctc(uint32_t milli)
{
        cli();
        TCCR1A = 0;
        TCCR1B = 0;
        // 1 / 256 prescaling
        SET_BIT(TCCR1B, WGM12);
        SET_BIT(TCCR1B, CS12);
        // cnt = (F_CPU/256 - 1)*quantity.
        OCR1A = (F_CPU/256 - 1)*milli/1000;
        // enable timer1.
        SET_BIT(TIMSK, OCIE1A);
        sei();
}
