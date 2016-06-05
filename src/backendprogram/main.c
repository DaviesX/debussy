#include <avr.h>
#include <spiioexp.h>
#include <extsram.h>
#include <hidusb.h>

int main()
{
        avr_init();
        hidusb_sys_init();
        spiioexp_sys_init();
        extsram_sys_init();
#ifdef DEBUG
        // spiioexp_test_blink_led();
        // extsram_test_read_write();
        hidusb_print_test();
#endif // DEBUG

        while(1) {
                wdt_reset();
                continue;
        }
        return 0;
}
