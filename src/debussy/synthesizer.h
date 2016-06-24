#ifndef SYNTHESIZER_H_INCLUDED
#define SYNTHESIZER_H_INCLUDED

/*
 * <note> decl
 */
enum NoteInstrument {
        NoteInstrSynSine,
        NoteInstrPiano,
};

struct note {
        uint8_t i_semi;
        uint8_t instrument;
        uint8_t force;
        float   t;
};

/*
 * <note> public
 */
void            note_init(struct note* self, uint8_t instrument, uint8_t i_semi, uint8_t force);
void            note_free(struct note* self);


/*
 * <synth> decl
 */
enum SyntheTarget {
        SyntheTargetAudioInterface,
        SyntheTargetWaveFile,
};

struct synth;

typedef void    (*f_Syn_Free) (struct synth* self);
typedef void    (*f_Syn_Run) (struct synth* self);
typedef void    (*f_Syn_Stop) (struct synth* self);

struct synth {
        f_Syn_Free      f_free;
        f_Syn_Run       f_run;
        f_Syn_Stop      f_stop;
        float           freq[88];
        struct note*    notes[16];
        uint8_t         n_notes;
};

/*
 * <synth> public
 */
void            syn_init(struct synth* self,
                         f_Syn_Free f_free,
                         f_Syn_Run f_run,
                         f_Syn_Stop f_stop);
void            syn_free(struct synth* self);
float           syn_get_freq(struct synth* self, uint8_t semi);
uint8_t         syn_hit_note(struct synth* self, struct note* note);
void            syn_release_note(struct synth* self, uint8_t ref);
void            syn_run(struct synth* self);
void            syn_stop(struct synth* self);


/*
 * <syn_audioif> decl
 */
struct syn_audioif {
        struct synth    __parent;
};

/*
 * <syn_audioif> decl
 */
void    syn_aif_init(struct syn_audioif* self);
void    syn_aif_free(struct syn_audioif* self);
void    syn_aif_run(struct syn_audioif* self);
void    syn_aif_stop(struct syn_audioif* self);


#endif // SYNTHESIZER_H_INCLUDED
