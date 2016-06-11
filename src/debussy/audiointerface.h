#ifndef AUDIOINTERFACE_H_INCLUDED
#define AUDIOINTERFACE_H_INCLUDED

/*
 * <audiointerface> public
 */
void audioif_du();
void audioif_on();
void audioif_square_wave_software(uint16_t freq, uint16_t duration, float volume);
void audioif_square_wave(uint16_t freq, uint16_t duration, float volume);
void audioif_off();


#endif // AUDIOINTERFACE_H_INCLUDED
