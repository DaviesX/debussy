#include <avr.h>
#include <hidusb.h>
#include <scheduler.h>


void schd_run(f_Schedule_Idle f_idle, void* user_data)
{
        for (;;) {
                wdt_reset();
                hidusb_tick();
                if (!f_idle || !f_idle(user_data)) {
/* @todo (davis#1#): <scheduler> put device into sleep mode. */
                }
        }
}
