#include <avr.h>
#include <spiioexp.h>
#include <extsram.h>


int main()
{
        spiioexp_sys_init();
        extsram_sys_init();
#ifdef DEBUG
        // spiioexp_test_blink_led();
        extsram_test_read_write();
#endif // DEBUG

        while(1) {
                continue;
        }
        return 0;
}
