#include <avr.h>
#include <filesystem.h>
#include <connection.h>
#include <audiointerface.h>
#include <synthesizer.h>
#include <musicsheet.h>


int main()
{
        // filesys_test_connect_directory();
        // conn_local_test_get_path();
        // audioif_test_sine_wave_441_64();
        syn_test_multi_notes_sine();
        // music_sheet_test_play_midi_file();
        return 0;
}
