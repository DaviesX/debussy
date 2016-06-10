#include <avr.h>
#include <spiioexp.h>
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
        spiioexp_sys_init();
        hidusb_puts("Device has been initialized. It's now connected to the host.");
        schd_run(__idle, nullptr);
        return 0;
}
