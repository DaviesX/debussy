#include <types.h>
#include <audiointerface.h>

#ifndef ARCH_X86_64
#  include <avr.h>

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

void audioif_set_24bit_wave_avr(f_Audioif_Fn_Mono_FP16 fn, void* user_data)
{
}

uint16_t audioif_sampling_rate_avr()
{
        return 44100;
}

#else
#  include <assert.h>
#  include <unistd.h>
#  include "../../lib/portaudio.h"

static f_Audioif_Fn_Mono_Float  g_fn_float = nullptr;
static PaStream*                g_stream = nullptr;

static int __audioif_write_floats(const void *in, void *out, unsigned long num_samples,
                                  const PaStreamCallbackTimeInfo* time_info,
                                  PaStreamCallbackFlags status_flags,
                                  void *user_data)
{
        if (g_fn_float)
                g_fn_float((float*) out, num_samples, user_data);
        return 0;
}

bool audioif_on_64()
{
        return paNoError == Pa_Initialize();
}

bool audioif_off_64()
{
        return (!g_stream || (paNoError == Pa_StopStream(g_stream))) &&
                paNoError == Pa_Terminate();
}

bool audioif_set_24bit_wave_64(f_Audioif_Fn_Mono_Float fn, void* user_data)
{

        PaError err;

        g_fn_float = fn;
        err = Pa_OpenDefaultStream(&g_stream, 0, 1, paFloat32,
                                   audioif_sampling_rate_64(), 4096,
                                   __audioif_write_floats, user_data);
        if (err != paNoError)
                return false;
        return paNoError == Pa_StartStream(g_stream);
}

uint16_t audioif_sampling_rate_64()
{
        return 44100;
}

/*
 * <audiointerface> test cases.
 */
#include <math.h>
#include <stdio.h>

struct wave_data {
        float   freq;
        float   rate;
        float   t;
};

static void __audioif_sine_wave(float *out, size_t num_samples, void* user_data)
{
        struct wave_data* data = (struct wave_data*) user_data;

        float dt = 1/data->rate;
        float w = 2*M_PI*data->freq;
        int i;
        for (i = 0; i < num_samples; i ++) {
                out[i] = sin(w*(data->t + i*dt));
        }
        data->t += num_samples*dt;
        int n_cycles = data->t*w/(2*M_PI);
        data->t -= n_cycles*2*M_PI/w;
}

void audioif_test_sine_wave_441_64()
{
        assert(audioif_on());

        struct wave_data data;
        data.freq = 440;
        data.rate = audioif_sampling_rate();
        data.t = 0;

        audioif_set_24bit_wave(__audioif_sine_wave, &data);

        sleep(2);

        assert(audioif_off());
}


#endif // ARCH_X86_64
