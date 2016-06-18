#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <console.h>
#include <filesystem.h>
#include <connection.h>
#include <connmgr.h>


/*
 * <conn_manager> public
 */
void connmgr_init(struct conn_manager* self)
{
        memset(self, 0, sizeof(*self));
        self->num_conns = 0;
}

void connmgr_free(struct conn_manager* self)
{
        size_t i;
        for (i = 0; i < connmgr_size(self); i ++) {
                conn_disconnect(self->conns[i]);
                conn_free(self->conns[i]), free(self->conns[i]);
        }
        free(self->conns);
        memset(self, 0, sizeof(*self));
}

size_t connmgr_size(const struct conn_manager* self)
{
        return self->num_conns;
}

void __connmgr_add_connection(struct conn_manager* self, struct connection* conn)
{
        self->conns = realloc(self->conns, (self->num_conns + 1)*sizeof(*self->conns));
        self->conns[self->num_conns ++] = conn;
}

void __connmgr_remove_connection_at(struct conn_manager* self, size_t i)
{
        conn_disconnect(self->conns[i]);
        conn_free(self->conns[i]), free(self->conns[i]);
        size_t j;
        for (j = i + 1; j < connmgr_size(self); j ++) {
                self->conns[j - 1] = self->conns[j];
        }
        self->num_conns --;
}

#ifdef ARCH_X86_64
#  include <linux/types.h>
#  include <linux/input.h>
#  include <linux/hidraw.h>
#  include <sys/ioctl.h>
#  include <sys/types.h>
#  include <sys/stat.h>
#  include <fcntl.h>
#  include <unistd.h>
#  include <errno.h>
#  include <libudev.h>

bool connmgr_add_scanned_avr_connections(struct conn_manager* self, struct console* console)
{
        // Remove old connections.
        size_t i;
        for (i = 0; i < connmgr_size(self); i ++) {
                if (conn_is_of_type(self->conns[i], ConnectionHost2Avr)) {
                      __connmgr_remove_connection_at(self, i);
                }
        }

        struct udev* udev_ctx = udev_new();
        if (!udev_ctx) {
                console_log(console, ConsoleLogSevere, "udev context is invalid. Failed to scan connections.");
                return false;
        }

        // Enumerate devices that are in the hidraw subsystem.
        struct udev_enumerate* enumerate = udev_enumerate_new(udev_ctx);
        udev_enumerate_add_match_subsystem(enumerate, "hidraw");
        udev_enumerate_scan_devices(enumerate);
        struct udev_list_entry* first_devent = udev_enumerate_get_list_entry(enumerate);

        // Scan all the devices.
        struct udev_list_entry* devent;
        for (devent = first_devent; devent != nullptr; devent = udev_list_entry_get_next(devent)) {
                // Get device path.
                const char* path = udev_list_entry_get_name(devent);
                struct udev_device* dev = udev_device_new_from_syspath(udev_ctx, path);

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

                // Add new connection.
                char combined_id[64];
                sprintf(combined_id, "vid=%s pid=%s", vender_id, product_id);
                __connmgr_add_connection(self,
                                         &conn_h2a_create(console,
                                                          combined_id,
                                                          product_name,
                                                          manufacturer,
                                                          node_path)->__parent);
        }
        udev_enumerate_unref(enumerate);
        udev_unref(udev_ctx);

        return true;
}

bool connmgr_add_local_connection(struct conn_manager* self, struct filesystem* base, struct console* console)
{
        struct conn_local* conn = conn_local_create(console, base);
        if (!conn) {
                const char* base_info = filesys_2string(base);
                console_log(console, ConsoleLogSevere, "Failed to create local connection on %s",
                            base_info), free((void*) base_info);
                return false;
        }
        connmgr_remove_connection(self, conn->__parent.id);
        __connmgr_add_connection(self, &conn->__parent);
        console_log(console, ConsoleLogNormal, "Local Connection %s", conn_local_2string(conn));
        return true;
}

#endif // ARCH_X86_64

void connmgr_remove_connection(struct conn_manager* self, const char* id)
{
        if (id == nullptr)
                return;
        size_t i;
        for (i = 0; i < connmgr_size(self); i ++) {
                if (conn_is_equal(self->conns[i], id)) {
                        __connmgr_remove_connection_at(self, i);
                }
        }
}

struct connection* connmgr_get_connection(const struct conn_manager* self, const char* id)
{
        if (id == nullptr)
                return nullptr;
        size_t i;
        for (i = 0; i < connmgr_size(self); i ++) {
                if (conn_is_equal(self->conns[i], id)) {
                        return self->conns[i];
                }
        }
        return nullptr;
}

struct connection* connmgr_get_connection_at(const struct conn_manager* self, size_t i)
{
        return self->conns[i];
}

const struct connection** connmgr_get_all_connections(const struct conn_manager* self)
{
        return (const struct connection**) self->conns;
}

const char** connmgr_2strings(const struct conn_manager* self)
{
        const char** strings = malloc(sizeof(char*)*connmgr_size(self));
        size_t i;
        for (i = 0; i < connmgr_size(self); i ++) {
                strings[i] = conn_2string(self->conns[i]);
        }
        return strings;
}
