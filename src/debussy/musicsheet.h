#ifndef MUSICSHEET_H_INCLUDED
#define MUSICSHEET_H_INCLUDED

#include <types.h>
#include <memory.h>
#include <synthesizer.h>

struct note {
        uint8_t         i_semi;
        uint8_t         length;
        uint8_t         force;
};

/*
 * <music_sheet_iter>
 */
struct music_sheet_iter {
        int             n_ticks;
};

/*
 * <music_sheet> decl
 */
struct file;
struct music_sheet {
        struct note*    note_q;
        uint16_t        front;
        uint16_t        rear;
        mem_addr_t      all_notes;
        uint16_t        n_notes;
        uint16_t        tempo;
};

/*
 * <music_sheet> public
 */
void            music_sheet_init(struct music_sheet* self);
void            music_sheet_free(struct music_sheet* self);
void            music_sheet_load_from_midi_file(struct music_sheet* self, struct file* file);
struct note*    music_sheet_get_notes(struct music_sheet* self, uint32_t* num_notes);
float           music_sheet_get_time_length(struct music_sheet* self);
void            music_sheet_get_iter_by_progress(struct music_sheet* self, float progress, struct music_sheet_iter* iter);
void            music_sheet_get_iter_by_note_id(struct music_sheet* self, uint32_t note_id, struct music_sheet_iter* iter);
void            music_sheet_insert_note_at(struct music_sheet* self, struct music_sheet_iter* iter, struct note* note);
void            music_sheet_remove_note_at(struct music_sheet* self, struct music_sheet_iter* iter);
bool            music_sheet_play_next(struct music_sheet* self, struct music_sheet_iter* iter, struct synth* synth);
void            music_sheet_set_tempo(struct music_sheet* self, uint8_t bpm);

/*
 * <music_sheet> test cases
 */
void            music_sheet_test_play_midi_file();


#endif // MUSICSHEET_H_INCLUDED
