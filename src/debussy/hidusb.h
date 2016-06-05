#ifndef HIDUSB_H_INCLUDED
#define HIDUSB_H_INCLUDED

/*
 * <hidusb> decl
 */
struct hidusb {
};

/*
 * <hidusb> public
 */
void hidusb_sys_init();
void hidusb_init(struct hidusb* self);
void hidusb_free(struct hidusb* self);
void hidusb_puts(struct hidusb* self, const char* s);

/*
 * <hidusb> test cases
 */
void hidusb_print_test();


#endif // HIDUSB_H_INCLUDED
