#include <avr.h>
#include <spiioexp.h>
#include <extsram.h>
#include <hidusb.h>
#include <scheduler.h>


static bool __idle(void* user_data)
{
        return true;
}

int main()
{
        avr_init();
        hidusb_sys_init(true);
        hidusb_puts("AVR has been initialized...");
        hidusb_puts("HIDUSB device has been initialized...");
        spiioexp_sys_init();
        hidusb_puts("SPI GPIO Expander has been initialized...");
        extsram_sys_init();
        hidusb_puts("External SRAM has been initialized...");
#ifdef DEBUG
        // spiioexp_test_blink_led();
        // extsram_test_read_write();
        // hidusb_print_test();
#endif // DEBUG

        schd_run(__idle, nullptr);
        return 0;
}
