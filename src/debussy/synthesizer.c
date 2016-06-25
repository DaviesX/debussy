#include <math.h>
#include <types.h>
#include <synthesizer.h>


/*
 * <syn_note> public
 */
void syn_note_init(struct syn_note* self, uint8_t instrument, uint8_t i_semi, uint8_t force)
{
        self->instrument = instrument;
        self->i_semi = i_semi;
        self->force = force;
        self->t = 0.0f;
}

void syn_note_free(struct syn_note* self)
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
        for (i = -48; i <= 39; i ++)
                self->freq[i + 48] = FREQ_A4*pow(2, (double) i/12.0f);

        for (i = 0; i < sizeof(self->notes)/sizeof(self->notes[0]); i ++)
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

uint8_t syn_hit_note(struct synth* self, struct syn_note* note)
{
        uint8_t i;
        for (i = 0; i < sizeof(self->notes)/sizeof(self->notes[0]); i ++)
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
#include <audiointerface.h>

void syn_aif_init(struct syn_audioif* self)
{
        self->is_on = false;

        syn_init(&self->__parent,
                 (f_Syn_Free) syn_aif_free,
                 (f_Syn_Run) syn_aif_run,
                 (f_Syn_Stop) syn_aif_stop);
}

void syn_aif_free(struct syn_audioif* self)
{
        syn_aif_stop(self);
}

static void __syn_aif_sine_synth(float *out, size_t num_samples, void* user_data)
{
        struct syn_audioif* self = (struct syn_audioif*) user_data;

        float dt = 1.0f/audioif_sampling_rate();
        float w[16];
        float weight[16];
        struct syn_note* notes[16];
        float sum = 0.0f;
        int j, k;
        for (j = 0, k = 0; j < 16; j ++) {
                struct syn_note* note = self->__parent.notes[j];
                if (note) {
                        notes[k] = note;
                        w[k] = 2*M_PI*syn_get_freq(&self->__parent, note->i_semi);
                        weight[k] = note->force;
                        sum += note->force;
                        k ++;
                }
        }

        for (j = 0; j < k; j ++)
                weight[j] /= sum;

        int i;
        for (i = 0; i < num_samples; i ++) {
                float fn = 0.0f;
                for (j = 0; j < k; j ++) {
                        float fade = exp(-(notes[j]->t)*(notes[j]->t));
                        fn += fade*weight[j]*sin(w[j]*(notes[j]->t + i*dt));
                }
                out[i] = fn;
        }

        for (j = 0; j < k; j ++)
                notes[j]->t += num_samples*dt;
}

void syn_aif_run(struct syn_audioif* self)
{
        self->is_on = true;
        audioif_on();
        audioif_set_24bit_wave(__syn_aif_sine_synth, self);
}

void syn_aif_stop(struct syn_audioif* self)
{
        if (self->is_on) {
                audioif_off();
                self->is_on = false;
        }
}

/*
 * <syn_audioif> test cases
 */
#include <unistd.h>

void syn_test_multi_notes_sine()
{
        struct syn_audioif syn;

        syn_aif_init(&syn);
        syn_run(&syn.__parent);

        struct syn_note n0, n1, n2;
        uint8_t rn0, rn1, rn2;
        syn_note_init(&n0, SynNoteInstrSynSine, 39, 100);
        syn_note_init(&n1, SynNoteInstrSynSine, 43, 100);
        syn_note_init(&n2, SynNoteInstrSynSine, 46, 100);

        rn0 = syn_hit_note(&syn.__parent, &n0);
        rn1 = syn_hit_note(&syn.__parent, &n1);
        rn2 = syn_hit_note(&syn.__parent, &n2);
        sleep(2);
        syn_release_note(&syn.__parent, rn0);
        syn_release_note(&syn.__parent, rn1);
        syn_release_note(&syn.__parent, rn2);

        syn_note_init(&n0, SynNoteInstrSynSine, 38, 100);
        syn_note_init(&n1, SynNoteInstrSynSine, 43, 100);
        syn_note_init(&n2, SynNoteInstrSynSine, 46, 20);
        rn0 = syn_hit_note(&syn.__parent, &n0);
        rn1 = syn_hit_note(&syn.__parent, &n1);
        rn2 = syn_hit_note(&syn.__parent, &n2);
        sleep(2);
        syn_release_note(&syn.__parent, rn0);
        syn_release_note(&syn.__parent, rn1);
        syn_release_note(&syn.__parent, rn2);

        syn_note_init(&n0, SynNoteInstrSynSine, 36, 100);
        syn_note_init(&n1, SynNoteInstrSynSine, 43, 180);
        syn_note_init(&n2, SynNoteInstrSynSine, 48, 100);
        rn0 = syn_hit_note(&syn.__parent, &n0);
        rn1 = syn_hit_note(&syn.__parent, &n1);
        rn2 = syn_hit_note(&syn.__parent, &n2);
        sleep(2);
        syn_release_note(&syn.__parent, rn0);
        syn_release_note(&syn.__parent, rn1);
        syn_release_note(&syn.__parent, rn2);

         syn_stop(&syn.__parent);

         syn_free(&syn.__parent);
}
