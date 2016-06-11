#ifndef AUDIOINTERFACE_H_INCLUDED
#define AUDIOINTERFACE_H_INCLUDED

/*
 * <audiointerface> public
 */
void audioif_du_avr();
void audioif_on_avr();
void audioif_off_avr();
void audioif_output_1bit_sw_avr(uint16_t freq, uint16_t duration, float volume);
void audioif_output_1bit_avr(uint16_t freq, uint16_t duration, float volume);
void audioif_output_24bit_wave_avr(float fn);

void audioif_on_x64();
void audioif_off_x64();
void audioif_output_24bit_wave_x64(float fn);

#ifndef ARCH_X86_64
#  define audioif_on                    audioif_on_avr
#  define audioif_off                   audioif_off_avr
#  define audioif_output_24bit_wave     audioif_output_24bit_wave_avr
#else
#  define audioif_on                    audioif_on_x64
#  define audioif_off                   audioif_off_x64
#  define audioif_output_24bit_wave     audioif_output_24bit_wave_x64
#endif // ARCH_X86_64


#endif // AUDIOINTERFACE_H_INCLUDED
