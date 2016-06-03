#ifndef USB_H_INCLUDED
#define USB_H_INCLUDED

#include <aerial.h>

/*
 * <usb> decl
 */
struct usb {
};

/*
 * <usb> public
 */
void    usb_init(struct usb* self);
void    usb_free(struct usb* self);
char*   usb_fetch_console_string(struct usb* self, size_t* num_bytes);


#endif // USB_H_INCLUDED
