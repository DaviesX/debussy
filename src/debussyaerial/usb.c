#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <libudev.h>
#include <aerial.h>
#include <console.h>
#include <usb.h>


/*
 * <usb_connection> public
 */
void usbconn_init(struct usb_connection* self,
                  const char* vender_id,
                  const char* name,
                  const char* manufacturer,
                  const char* dev_node_path)
{
        memset(self, 0, sizeof(*self));
        self->vender_id         = vender_id != nullptr ? strdup(vender_id) : strdup("");
        self->name              = name != nullptr ? strdup(name) : strdup("");
        self->manufacturer      = manufacturer != nullptr ? strdup(manufacturer) : strdup("");
        self->dev_node_path     = dev_node_path != nullptr ? strdup(dev_node_path) : strdup("");
}

void usbconn_free(struct usb_connection* self)
{
        free(self->vender_id);
        free(self->name);
        free(self->manufacturer);
        free(self->dev_node_path);
        memset(self, 0, sizeof(*self));
}

struct usb_connection* usbconns_find_by_vender_id(struct usb_connection* conns, int n_conns, const char* id)
{
        int i;
        for (i = 0; i < n_conns; i ++) {
                if (!strcmp(conns[i].vender_id, id)) {
                        return &conns[i];
                }
        }
        return nullptr;
}

char** usbconns_format_as_strings(struct usb_connection* conns, int n_conns)
{
        char** strings = malloc(sizeof(char*)*n_conns);
        int i;
        for (i = 0; i < n_conns; i ++) {
                struct usb_connection* conn = &conns[i];
                int len = strlen(conn->vender_id) +
                          strlen(conn->name) +
                          strlen(conn->manufacturer) +
                          strlen(conn->dev_node_path) + 20;
                strings[i] = malloc(len*sizeof(char));
                sprintf(strings[i], "%s by %s(%s): %s",
                                    conn->name, conn->manufacturer, conn->vender_id, conn->dev_node_path);
        }
        return strings;
}

/*
 * <usb> public
 */
void usb_init(struct usb* self)
{
        memset(self, 0, sizeof(*self));
        self->udev_ctx = udev_new();
}

void usb_free(struct usb* self)
{
        if (self->udev_ctx != nullptr)
                udev_unref(self->udev_ctx);
        memset(self, 0, sizeof(*self));
}

struct usb_connection* usb_scan_connections(const struct usb* self, int* num_conns, struct console* console)
{
        *num_conns = 0;

        if (!self->udev_ctx) {
                console_log(console, ConsoleLogSevere, "udev context is invalid. Failed to scan connections.");
                return nullptr;
        }

        // Enumerate devices that are in the hidraw subsystem.
        struct udev_enumerate* enumerate = udev_enumerate_new(self->udev_ctx);
        udev_enumerate_add_match_subsystem(enumerate, "hidraw");
        udev_enumerate_scan_devices(enumerate);
        struct udev_list_entry* first_devent = udev_enumerate_get_list_entry(enumerate);

        // Scan all the devices.
        struct udev_list_entry* devent;
        int n_devs = 0;
        struct usb_connection* conns = nullptr;
        for (devent = first_devent; devent != nullptr; devent = udev_list_entry_get_next(devent)) {
                // Get device path.
                const char* path = udev_list_entry_get_name(devent);
                struct udev_device* dev = udev_device_new_from_syspath(self->udev_ctx, path);

                // Get device node path.
                const char* node_path = udev_device_get_devnode(dev);
                console_log(console, ConsoleLogNormal, "Device node path: %s.", node_path);

                // Get parent usb device.
                dev = udev_device_get_parent_with_subsystem_devtype(dev, "usb", "usb_device");
                if (!dev) {
                        console_log(console, ConsoleLogNormal,
                                    "Unable to find parent usb device. This device will be skipped.");
                        continue;
                }

                // Extract device infos.
                const char* vender_id = udev_device_get_sysattr_value(dev, "idVendor");
                const char* product_id = udev_device_get_sysattr_value(dev, "idProduct");
                const char* manufacturer = udev_device_get_sysattr_value(dev,"manufacturer");
                const char* product_name = udev_device_get_sysattr_value(dev,"product");

                console_log(console, ConsoleLogNormal, "\tVender ID & Product ID: %s %s", vender_id, product_id);
                console_log(console, ConsoleLogNormal, "\tManufacturer: %s", manufacturer);
                console_log(console, ConsoleLogNormal, "\tProduct Name: %s", product_name);
                udev_device_unref(dev);

                conns = realloc(conns, ++ n_devs*sizeof(*conns));
                usbconn_init(&conns[n_devs - 1], vender_id, product_name, manufacturer, node_path);
        }
        udev_enumerate_unref(enumerate);
        *num_conns = n_devs;
        return conns;
}

bool usb_connect_to(struct usb* self, struct usb_connection* conn)
{
        return false;
}

char* usb_fetch_console_string(const struct usb* self, size_t* num_bytes)
{
        return nullptr;
}
