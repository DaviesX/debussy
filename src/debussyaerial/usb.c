#include <linux/types.h>
#include <linux/input.h>
#include <linux/hidraw.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
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
        self->fd                = -1;
}

void usbconn_free(struct usb_connection* self)
{
        free(self->vender_id);
        free(self->name);
        free(self->manufacturer);
        free(self->dev_node_path);
        memset(self, 0, sizeof(*self));
}

char* usbconn_format_as_string(struct usb_connection* self)
{
        int len = strlen(self->vender_id) +
                  strlen(self->name) +
                  strlen(self->manufacturer) +
                  strlen(self->dev_node_path) + 30;
        char* buf = malloc(len*sizeof(char));
        sprintf(buf, "%s manufactured by %s (%s): %s",
                     self->name, self->manufacturer, self->vender_id, self->dev_node_path);
        return buf;
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
                strings[i] = usbconn_format_as_string(conn);
        }
        return strings;
}

void usbconns_free_strings(char** strings, int n_conns)
{
        int i;
        for (i = 0; i < n_conns; i ++) {
                free(strings[i]);
        }
        free(strings);
}

/*
 * <usb> public
 */
void usb_init(struct usb* self)
{
        memset(self, 0, sizeof(*self));
        self->udev_ctx = udev_new();
        self->is_connected = false;
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
                char combined_id[64];
                sprintf(combined_id, "vid=%s pid=%s", vender_id, product_id);
                usbconn_init(&conns[n_devs - 1], combined_id, product_name, manufacturer, node_path);
        }
        udev_enumerate_unref(enumerate);
        *num_conns = n_devs;
        return conns;
}

bool usb_is_connected(struct usb* self)
{
        return self->is_connected;
}

bool usb_connect_to(struct usb* self, struct usb_connection* conn, struct console* console)
{
        // Close old file descriptor.
        if (conn->fd >= 0)
                close(conn->fd);
        // Open new device.
        char* conn_str = usbconn_format_as_string(conn);
        if (0 > (conn->fd = open(conn->dev_node_path, O_RDWR | O_NONBLOCK))) {
                console_log(console, ConsoleLogSevere, "Failed to open device %s", conn_str), free(conn_str);
                console_log(console, ConsoleLogSevere, "Cause: %s", strerror(errno));
                self->is_connected = false;
                return false;
        } else {
                console_log(console, ConsoleLogSevere, "Successfully opened device %s", conn_str), free(conn_str);
                self->conn = *conn;
                self->is_connected = true;
                return true;
        }
}

#define REPORT_BYTE_COUNT       65
#define REPORT_BUFFER_SIZE      (REPORT_BYTE_COUNT - 3)

struct request {
        uint8_t report_id;
        uint8_t type;
        uint8_t len;
        char    buf[REPORT_BUFFER_SIZE];
};

char* usb_fetch_console_string(const struct usb* self, size_t* num_bytes)
{
        if (!self->is_connected)
                return nullptr;

        int n_fetched, n_total = 0;
        struct request req_buf;
        char* buf = &req_buf;

        while (n_total < REPORT_BYTE_COUNT &&
               0 < (n_fetched = read(self->conn.fd, &buf[n_total], sizeof(buf)))) {
                n_total += n_fetched;
        }

        if (n_total > 0) {
                *num_bytes = req_buf.len;
                return strdup(req_buf.buf);
        } else {
                *num_bytes = 0;
                return nullptr;
        }
}
