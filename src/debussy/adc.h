#ifndef ADC_H_INCLUDED
#define ADC_H_INCLUDED

/*
 * adc public
 */
void adc_init();
extern uint8_t g_adc8;
inline uint8_t adc_get_value_8()
{
        return g_adc8;
}


#endif // ADC_H_INCLUDED
