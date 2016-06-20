#include <avr.h>
#include <spiioexp.h>
#include <hidusb.h>
#include <connection.h>
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

        struct conn_a2h conn;
        conn_a2h_init(&conn);

        conn_a2h_puts(&conn, "Device has been initialized. It's now connected to the host.");
        schd_run(__idle, nullptr);
        return 0;
}
