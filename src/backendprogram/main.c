#include <avr.h>
#include <spiioexp.h>
#include <extsram.h>
#include <hidusb.h>
#include <scheduler.h>
#include <chipselect.h>


static bool __idle(void* user_data)
{
        return true;
}

int main()
{
        avr_init();
        cs2_sys_init();
        spiioexp_sys_init();
        extsram_sys_init();
#ifdef DEBUG
        // spiioexp_test_blink_led();
        // extsram_test_read_write();
        // hidusb_print_test();
#endif // DEBUG

        schd_run(__idle, nullptr);
        return 0;
}
