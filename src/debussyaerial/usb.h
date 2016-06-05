#ifndef USB_H_INCLUDED
#define USB_H_INCLUDED

#include <aerial.h>
#include <console.h>

/*
 * <usb_connection> decl
 */
struct usb_connection {
        char*           vender_id;
        char*           name;
        char*           manufacturer;
        char*           dev_node_path;
};

/*
 * <usb_connection> public
 */
void                    usbconn_init(struct usb_connection* self,
                                     const char* vender_id,
                                     const char* name,
                                     const char* manufacturer,
                                     const char* dev_node_path);
void                    usbconn_free(struct usb_connection* self);
struct usb_connection*  usbconns_find_by_vender_id(struct usb_connection* conns, int n_conns, const char* id);
char**                  usbconns_format_as_strings(struct usb_connection* conns, int n_conns);


/*
 * <usb> decl
 */
struct udev;
struct usb {
        struct udev*            udev_ctx;
        struct usb_connection   conn;
};

/*
 * <usb> public
 */
void                    usb_init(struct usb* self);
void                    usb_free(struct usb* self);
struct usb_connection*  usb_scan_connections(const struct usb* self, int* num_conns, struct console* console);
bool                    usb_connect_to(struct usb* self, struct usb_connection* conn);
char*                   usb_fetch_console_string(const struct usb* self, size_t* num_bytes);


#endif // USB_H_INCLUDED
