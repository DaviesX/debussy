#include <avr.h>
#include <filesystem.h>
#include <connection.h>
#include <audiointerface.h>


int main()
{
        // filesys_test_connect_directory();
        // conn_local_test_get_path();
        audioif_test_sine_wave_441_64();
        return 0;
}
