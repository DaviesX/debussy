#ifndef AUDIOINTERFACE_H_INCLUDED
#define AUDIOINTERFACE_H_INCLUDED

#include <types.h>

/*
 * <audiointerface> public
 */
typedef void (*f_Audioif_Fn_Mono_Float) (float *output, size_t num_samples, void* user_data);
typedef void (*f_Audioif_Fn_Mono_FP16) (uint16_t *output, size_t num_samples, void* user_data);

void            audioif_du_avr();
void            audioif_on_avr();
void            audioif_off_avr();
void            audioif_output_1bit_sw_avr(uint16_t freq, uint16_t duration, float volume);
void            audioif_output_1bit_avr(uint16_t freq, uint16_t duration, float volume);
void            audioif_set_24bit_wave_avr(f_Audioif_Fn_Mono_FP16 fn, void* user_data);
uint16_t        audioif_sampling_rate_avr();

bool            audioif_on_64();
bool            audioif_off_64();
bool            audioif_set_24bit_wave_64(f_Audioif_Fn_Mono_Float fn, void* user_data);
uint16_t        audioif_sampling_rate_64();

#ifndef ARCH_X86_64
#  define audioif_on                    audioif_on_avr
#  define audioif_off                   audioif_off_avr
#  define audioif_set_24bit_wave        audioif_set_24bit_wave_avr
#  define audioif_sampling_rate         audioif_sampling_rate_avr
#else
#  define audioif_on                    audioif_on_64
#  define audioif_off                   audioif_off_64
#  define audioif_set_24bit_wave        audioif_set_24bit_wave_64
#  define audioif_sampling_rate         audioif_sampling_rate_64
#endif // ARCH_X86_64

/*
 * <audiointerface> test cases.
 */
void audioif_test_sine_wave_441_64();


#endif // AUDIOINTERFACE_H_INCLUDED
