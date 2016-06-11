#ifndef HIDUSB_H_INCLUDED
#define HIDUSB_H_INCLUDED

#ifndef ARCH_X86_64
#  include <avr.h>


/*
 * <hidusb> decl
 */
struct hidusb {
};

/*
 * <hidusb> public
 */
void hidusb_sys_init(bool dev_enable);
void hidusb_init(struct hidusb* self);
void hidusb_free(struct hidusb* self);
void hidusb_tick();
void hidusb_puts(const char* s);

/*
 * <hidusb> test cases
 */
void hidusb_print_test();

#endif // ARCH_X86_64


#endif // HIDUSB_H_INCLUDED
