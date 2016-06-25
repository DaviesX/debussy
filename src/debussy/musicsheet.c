#include <filesystem.h>
#include <memory.h>
#include <string.h>
#include <musicsheet.h>


/*
 * <music_sheet> public
 */
void music_sheet_init(struct music_sheet* self)
{
        memset(self, 0, sizeof(*self));
}

void music_sheet_free(struct music_sheet* self)
{
        memset(self, 0, sizeof(*self));
}

void music_sheet_load_from_midi_file(struct music_sheet* self, struct file* file)
{
}

struct note* music_sheet_get_notes(struct music_sheet* self, uint32_t* num_notes)
{
        return nullptr;
}

float music_sheet_get_time_length(struct music_sheet* self)
{
        return 0.0f;
}

void music_sheet_get_iter_by_progress(struct music_sheet* self, float progress, struct music_sheet_iter* iter)
{
}

void music_sheet_get_iter_by_note_id(struct music_sheet* self, uint32_t note_id, struct music_sheet_iter* iter)
{
}

void music_sheet_insert_note_at(struct music_sheet* self, struct music_sheet_iter* iter, struct note* note)
{
}

void music_sheet_remove_note_at(struct music_sheet* self, struct music_sheet_iter* iter)
{
}

bool music_sheet_play_next(struct music_sheet* self, struct music_sheet_iter* iter, struct synth* synth)
{
        return false;
}

void music_sheet_set_tempo(struct music_sheet* self, uint8_t bpm)
{
}

/*
 * <music_sheet> test cases
 */
#include <filesystem.h>
#include <synthesizer.h>

void music_sheet_test_play_midi_file()
{
        struct fs_posix fs;
        fs_posix_init(&fs, "data");
        struct file* file = filesys_open_file(&fs.__parent, "./piano_sonata_13_1_(c)oguri.mid", false);

        struct music_sheet sheet;
        music_sheet_init(&sheet);
        music_sheet_load_from_midi_file(&sheet, file);

        filesys_free(&fs.__parent);

        struct music_sheet_iter iter;
        music_sheet_get_iter_by_note_id(&sheet, 0, &iter);

        struct syn_audioif syn;
        syn_aif_init(&syn);

        while (music_sheet_play_next(&sheet, &iter, &syn.__parent))
                continue;

        music_sheet_free(&sheet);
        syn_aif_free(&syn);
}
