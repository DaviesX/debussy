#include <math.h>
#include <types.h>
#include <synthesizer.h>


/*
 * <note> public
 */
void note_init(struct note* self, uint8_t instrument, uint8_t i_semi, uint8_t force)
{
        self->instrument = instrument;
        self->i_semi = i_semi;
        self->force = force;
        self->t = 0.0f;
}

void note_free(struct note* self)
{
        // Do nothing.
}


/*
 * <synthe> public
 */
#define FREQ_A4         440

void syn_init(struct synth* self,
              f_Syn_Free f_free,
              f_Syn_Run f_run,
              f_Syn_Stop f_stop)
{
        self->f_free = f_free;
        self->f_run = f_run;
        self->f_stop = f_stop;

        int8_t i;
        for (i = -48; i <= 38; i ++)
                self->freq[i + 48] = FREQ_A4*pow(2, (double) i/12.0f);

        for (i = 0; i < sizeof(self->notes); i ++)
                self->notes[i] = nullptr;
}

void syn_free(struct synth* self)
{
        self->f_free(self);
}

float syn_get_freq(struct synth* self, uint8_t semi)
{
        return self->freq[semi];
}

uint8_t syn_hit_note(struct synth* self, struct note* note)
{
        uint8_t i;
        for (i = 0; i < sizeof(self->notes); i ++)
                if (self->notes[i] == nullptr)
                        break;
        self->notes[i] = note;
        self->n_notes ++;
        return i;
}

void syn_release_note(struct synth* self, uint8_t ref)
{
        self->notes[ref] = nullptr;
        self->n_notes --;
}

void syn_run(struct synth* self)
{
        self->f_run(self);
}

void syn_stop(struct synth* self)
{
        self->f_free(self);
}


/*
 * <syn_audioif> decl
 */
void syn_aif_init(struct syn_audioif* self)
{
        syn_init(&self->__parent,
                 (f_Syn_Free) syn_aif_free,
                 (f_Syn_Run) syn_aif_run,
                 (f_Syn_Stop) syn_aif_stop);
}

void syn_aif_free(struct syn_audioif* self)
{
}

void syn_aif_run(struct syn_audioif* self)
{
}

void syn_aif_stop(struct syn_audioif* self)
{
}
