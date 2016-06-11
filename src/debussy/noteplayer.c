#include <math.h>
#include <types.h>
#include <noteplayer.h>

#define WHOLE_NOTE      16
#define FREQ_A4         440

//static float note_player_get_freq(struct note_player* self, uint8_t semi)
//{
//        return self->freq[semi];
//}

void note_player_init(struct note_player* self)
{
        int8_t i;
        for (i = -48; i <= 38; i ++) {
                self->freq[i + 48] = FREQ_A4*pow(2, (double) i/12.0f);
        }
}
