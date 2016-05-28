#include <math.h>
#include <avr.h>
#include <noteplayer.h>

#define WHOLE_NOTE      16

#define FREQ_A4         440

static float g_freq[88];

static float note_player_get_freq(uint8_t semi)
{
        return g_freq[semi];
}

void note_player_init()
{
        int8_t i;
        for (i = -48; i <= 38; i ++) {
                g_freq[i + 48] = FREQ_A4*pow(2, (double) i/12.0f);
        }
}
