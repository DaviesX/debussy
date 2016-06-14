#ifndef HIDUSB_H_INCLUDED
#define HIDUSB_H_INCLUDED

#include <types.h>


#define REPORT_BYTE_COUNT       64
#define REPORT_BUFFER_SIZE      (REPORT_BYTE_COUNT - 2)

/*
 * <hidusb_request> decl
 */
struct hidusb_request {
        uint8_t report_id;
        uint8_t type;
        uint8_t len;
        char    buf[REPORT_BUFFER_SIZE];
};

/*
 * <hidusb_request> public
 */
void hidusb_request_init(struct hidusb_request* self, uint8_t type, uint8_t len);


#ifndef ARCH_X86_64
#  include <avr.h>


/*
 * <hidusb> public
 */
void hidusb_sys_init(bool dev_enable);
void hidusb_connect();
void hidusb_disconnect();
bool hidusb_is_enabled();
bool hidusb_is_connected();
void hidusb_tick();
void hidusb_put_request(struct hidusb_request* request);

/*
 * <hidusb> test cases
 */
void hidusb_print_test();

#endif // ARCH_X86_64


#endif // HIDUSB_H_INCLUDED
