#include <math.h>
#include <types.h>
#include <synthesizer.h>

#define FREQ_A4         440

float __syn_get_freq(struct synthesizer* self, uint8_t semi)
{
        return self->freq[semi];
}

void syn_init(struct synthesizer* self)
{
        int8_t i;
        for (i = -48; i <= 38; i ++) {
                self->freq[i + 48] = FREQ_A4*pow(2, (double) i/12.0f);
        }
}

void syn_free(struct synthesizer* self)
{
}
