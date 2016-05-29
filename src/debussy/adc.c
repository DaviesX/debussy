#include <avr.h>
#include <lcd.h>
#include <adc.h>

#define ADC_PORT        PORTA

void adc_init()
{
        cli();

        SET_BIT(ADMUX, ADLAR);
        //SET_BIT(ADMUX, REFS0);

        SET_BIT(ADCSRA, ADEN);
        // 50000 < f < 200000
        // 16000000/200000 < K < 16000000/50000
        // 80 < K < 320, should use the highest prescalar, 128
        SET_BIT(ADCSRA, ADPS0);
        SET_BIT(ADCSRA, ADPS1);
        SET_BIT(ADCSRA, ADPS2);

        SET_BIT(ADCSRA, ADIE);
        SET_BIT(ADCSRA, ADSC);

        sei();
}

uint8_t g_adc8 = 0;

ISR(ADC_vect)
{
        g_adc8 = ADCH;
        SET_BIT(ADCSRA, ADSC);
}
