#ifndef SYNTHESIZER_H_INCLUDED
#define SYNTHESIZER_H_INCLUDED

/*
 * <syn_note> decl
 */
enum SynNoteInstrument {
        SynNoteInstrSynSine,
        SynNoteInstrPiano,
};

struct syn_note {
        uint8_t i_semi;
        uint8_t instrument;
        uint8_t force;
        float   t;
};

/*
 * <syn_note> public
 */
void            syn_note_init(struct syn_note* self, uint8_t instrument, uint8_t i_semi, uint8_t force);
void            syn_note_free(struct syn_note* self);


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
        f_Syn_Free              f_free;
        f_Syn_Run               f_run;
        f_Syn_Stop              f_stop;
        float                   freq[88];
        struct syn_note*        notes[16];
        uint8_t                 n_notes;
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
uint8_t         syn_hit_note(struct synth* self, struct syn_note* note);
void            syn_release_note(struct synth* self, uint8_t ref);
void            syn_run(struct synth* self);
void            syn_stop(struct synth* self);


/*
 * <syn_audioif> decl
 */
struct syn_audioif {
        struct synth    __parent;
        bool            is_on;
};

/*
 * <syn_audioif> decl
 */
void    syn_aif_init(struct syn_audioif* self);
void    syn_aif_free(struct syn_audioif* self);
void    syn_aif_run(struct syn_audioif* self);
void    syn_aif_stop(struct syn_audioif* self);

/*
 * <syn_audioif> test cases
 */
void    syn_test_multi_notes_sine();



#endif // SYNTHESIZER_H_INCLUDED
