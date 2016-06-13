#ifndef MUSICSHEET_H_INCLUDED
#define MUSICSHEET_H_INCLUDED

#include <types.h>

/*
 * <note> decl
 */
struct note {
};

/*
 * <musicsheet_iter>
 */
struct musicsheet_iter {
};

/*
 * <musicsheet> decl
 */
struct file;
struct musicsheet {
};

/*
 * <musicsheet> public
 */
void            musicsheet_init(struct musicsheet* self);
void            musicsheet_free(struct musicsheet* self);
void            musicsheet_load_from_midi_file(struct musicsheet* self, struct file* file);
struct note*    musicsheet_get_notes(struct musicsheet* self, uint32_t* num_notes);
float           musicsheet_get_time_length(struct musicsheet* self);
void            musicsheet_get_iter_by_progress(struct musicsheet* self, float progress, struct musicsheet_iter* iter);
void            musicsheet_get_iter_by_note_id(struct musicsheet* self, uint32_t note_id, struct musicsheet_iter* iter);
void            musicsheet_insert_note_at(struct musicsheet* self, struct musicsheet_iter* iter, struct note* note);
void            musicsheet_remove_note_at(struct musicsheet* self, struct musicsheet_iter* iter);
void            musicsheet_play_next(struct musicsheet* self, struct musicsheet_iter* iter);
void            musicsheet_set_tempo(struct musicsheet* self, uint8_t bpm);

/*
 * <musicsheet> test cases
 */
void            musicsheet_test_play_midi_file();


#endif // MUSICSHEET_H_INCLUDED
