#ifndef SPEAKER_H_INCLUDED
#define SPEAKER_H_INCLUDED

/*
 * <speaker> public
 */
void speaker_du();
void speaker_on();
void speaker_square_wave_software(uint16_t freq, uint16_t duration, float volume);
void speaker_square_wave(uint16_t freq, uint16_t duration, float volume);
void speaker_off();


#endif // SPEAKER_H_INCLUDED
