#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <avr.h>
#include <actionprotocols.h>
#include <console.h>
#include <hidusb.h>
#include <filesystem.h>
#include <audioplayer.h>
#include <connection.h>


/*
 * <connection> public
 */
void conn_init(struct connection* self, struct console* console, const char* id, uint8_t type,
               f_Conn_Free f_free,
               f_Conn_Connect_To f_connect_to,
               f_Conn_Is_Connected f_is_connected,
               f_Conn_Disconnect f_disconnect,
               f_Conn_Get_Action f_get_action,
               f_Conn_Puts f_puts,
               f_Conn_Gets f_gets,
               f_Conn_Put_Directory_Structure f_put_directory_structure,
               f_Conn_Get_Directory_Structure f_get_directory_structure,
               f_Conn_Put_File f_put_file,
               f_Conn_Get_File f_get_file,
               f_Conn_Put_Audio_Player_State f_put_audio_player_state,
               f_Conn_Get_Audio_Player_State f_get_audio_player_state,
               f_Conn_2string f_2string)
{
        memset(self, 0, sizeof(*self));
        self->id = id == nullptr ? nullptr : strcpy(malloc(strlen(id) + 1), id);
        self->type = type;
        self->console = console;
        self->f_free = f_free;
        self->f_connect_to = f_connect_to;
        self->f_disconnect = f_disconnect;
        self->f_is_connected = f_is_connected;
        self->f_get_action = f_get_action;
        self->f_puts = f_puts;
        self->f_gets = f_gets;
        self->f_put_directory_structure = f_put_directory_structure;
        self->f_get_directory_structure = f_get_directory_structure;
        self->f_put_file = f_put_file;
        self->f_get_file = f_get_file;
        self->f_put_audio_player_state = f_put_audio_player_state;
        self->f_get_audio_player_state = f_get_audio_player_state;
        self->f_2string = f_2string;
}

void conn_free(struct connection* self)
{
        self->f_free(self), free(self->id), memset(self, 0, sizeof(*self));
}

bool conn_connect_to(struct connection* self)
{
        return self->f_connect_to(self);
}

bool conn_disconnect(struct connection* self)
{
        return self->f_disconnect(self);
}

bool conn_is_connected(struct connection* self)
{
        return self->f_is_connected(self);
}

struct action_protocol* conn_get_action(struct connection* self, bool is_blocking)
{
        return self->f_get_action(self, is_blocking);
}

void conn_puts(struct connection* self, const char* s)
{
        self->f_puts(self, s);
}

char* conn_gets(struct connection* self)
{
        return self->f_gets(self);
}

void conn_put_directory_structure(struct connection* self, struct directory* src)
{
        self->f_put_directory_structure(self, src);
}

void conn_get_directory_structure(struct connection* self, struct directory* dst)
{
        self->f_get_directory_structure(self, dst);
}

void conn_put_file(struct connection* self, struct file* src)
{
        self->f_put_file(self, src);
}

void conn_get_file(struct connection* self, struct file* dst)
{
        self->f_get_file(self, dst);
}

void conn_put_audio_player_state(struct connection* self, struct audioplayer_state* aps)
{
        self->f_put_audio_player_state(self, aps);
}

void conn_get_audio_player_state(struct connection* self, struct audioplayer_state* aps)
{
        self->f_get_audio_player_state(self, aps);
}

char* conn_2string(struct connection* self)
{
        return self->f_2string(self);
}

#ifndef ARCH_X86_64

/*
 * <conn_a2h> public
 */
struct conn_a2h* conn_a2h_create()
{
        struct conn_a2h* self = malloc(sizeof(*self));
        conn_a2h_init(self);
        return self;
}

void conn_a2h_init(struct conn_a2h* self)
{
        memset(self, 0, sizeof(*self));
        conn_init(&self->__parent, nullptr, nullptr, ConnectionAvr2Host,
                  (f_Conn_Free) conn_a2h_free,
                  (f_Conn_Connect_To) conn_a2h_connect_to,
                  (f_Conn_Disconnect) conn_a2h_disconnect,
                  (f_Conn_Is_Connected) conn_a2h_is_connected,
                  (f_Conn_Get_Action) conn_a2h_get_action,
                  (f_Conn_Puts) conn_a2h_puts,
                  (f_Conn_Gets) conn_a2h_gets,
                  (f_Conn_Put_Directory_Structure) conn_a2h_put_directory_structure,
                  (f_Conn_Get_Directory_Structure) conn_a2h_get_directory_structure,
                  (f_Conn_Put_File)conn_a2h_put_file,
                  (f_Conn_Get_File) conn_a2h_get_file,
                  (f_Conn_Put_Audio_Player_State) conn_a2h_put_audio_player_state,
                  (f_Conn_Get_Audio_Player_State) conn_a2h_get_audio_player_state,
                  (f_Conn_2string) conn_a2h_2string);
}

void conn_a2h_free(struct conn_a2h* self)
{
        conn_a2h_disconnect(self);
}

bool conn_a2h_connect_to(struct conn_a2h* self)
{
        hidusb_connect();
        return true;
}

bool conn_a2h_disconnect(struct conn_a2h* self)
{
        hidusb_disconnect();
        return true;
}

bool conn_a2h_is_connected(struct conn_a2h* self)
{
        return hidusb_is_connected();
}

struct action_protocol* conn_a2h_get_action(struct conn_a2h* self, bool is_blocking)
{
        abort();
}

void conn_a2h_puts(struct conn_a2h* self, const char* s)
{
        if (!conn_a2h_is_connected(self))
                return;
        // Prepare request
        uint8_t len = strlen(s) + 1;
        struct hidusb_request request;
        hidusb_request_init(&request, ActionPutString, len);
        memcpy(request.buf, s, len);
        // Send request.
        hidusb_put_request(&request);
}

char* conn_a2h_gets(struct conn_a2h* self)
{
        abort();
}

void conn_a2h_put_directory_structure(struct conn_a2h* self, struct directory* src)
{
        abort();
}

void conn_a2h_get_directory_structure(struct conn_a2h* self, struct directory* dst)
{
        abort();
}

void conn_a2h_put_file(struct conn_a2h* self, struct file* src)
{
        abort();
}

void conn_a2h_get_file(struct conn_a2h* self, struct file* dst)
{
        abort();
}

void conn_a2h_put_audio_player_state(struct conn_a2h* self, struct audioplayer_state* aps)
{
        abort();
}

void conn_a2h_get_audio_player_state(struct conn_a2h* self, struct audioplayer_state* aps)
{
        abort();
}

char* conn_a2h_2string(struct conn_a2h* self)
{
        return "";
}

/*
 * <conn_a2h> test cases
 */
void conn_a2h_puts_test()
{
        struct conn_a2h conn;
        conn_a2h_init(&conn);
        conn_a2h_connect_to(&conn);

        while(1) {
                conn_a2h_puts(&conn, "hello world");
                hidusb_tick();
                wdt_reset();
        }
}

#else
#  include <sys/ioctl.h>
#  include <sys/types.h>
#  include <sys/stat.h>
#  include <fcntl.h>
#  include <unistd.h>
#  include <errno.h>

/*
 * <conn_h2a> public
 */
struct conn_h2a* conn_h2a_create(struct console* console,
                                 const char* vender_id,
                                 const char* name,
                                 const char* manufacturer,
                                 const char* dev_node_path)
{
        struct conn_h2a* self = malloc(sizeof(*self));
        conn_h2a_init(self, console, vender_id, name, manufacturer, dev_node_path);
        return self;
}

void conn_h2a_init(struct conn_h2a* self,
                   struct console* console,
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
        conn_init(&self->__parent, console, self->vender_id, ConnectionHost2Avr,
                  (f_Conn_Free) conn_h2a_free,
                  (f_Conn_Connect_To) conn_h2a_connect_to,
                  (f_Conn_Disconnect) conn_h2a_disconnect,
                  (f_Conn_Is_Connected) conn_h2a_is_connected,
                  (f_Conn_Get_Action) conn_h2a_get_action,
                  (f_Conn_Puts) conn_h2a_puts,
                  (f_Conn_Gets) conn_h2a_gets,
                  (f_Conn_Put_Directory_Structure) conn_h2a_put_directory_structure,
                  (f_Conn_Get_Directory_Structure) conn_h2a_get_directory_structure,
                  (f_Conn_Put_File)conn_h2a_put_file,
                  (f_Conn_Get_File) conn_h2a_get_file,
                  (f_Conn_Put_Audio_Player_State) conn_h2a_put_audio_player_state,
                  (f_Conn_Get_Audio_Player_State) conn_h2a_get_audio_player_state,
                  (f_Conn_2string) conn_h2a_2string);
}

void conn_h2a_free(struct conn_h2a* self)
{
        free(self->vender_id);
        free(self->name);
        free(self->manufacturer);
        free(self->dev_node_path);
        memset(self, 0, sizeof(*self));
}

bool conn_h2a_connect_to(struct conn_h2a* self)
{
        struct console* console = self->__parent.console;

        // Close old device.
        conn_h2a_disconnect(self);

        // Open new device.
        char* conn_str = conn_h2a_2string(self);
        if (0 > (self->fd = open(self->dev_node_path, O_RDWR | O_NONBLOCK))) {
                console_log(console, ConsoleLogSevere, "Failed to open device %s", conn_str), free(conn_str);
                console_log(console, ConsoleLogSevere, "Cause: %s", strerror(errno));
                return false;
        } else {
                console_log(console, ConsoleLogSevere, "Successfully opened device %s", conn_str), free(conn_str);
                return true;
        }
}

bool conn_h2a_disconnect(struct conn_h2a* self)
{
        struct console* console = self->__parent.console;

        if (!conn_h2a_is_connected(self)) {
                console_log(console, ConsoleLogNormal, "Nothing to disconnect");
                return true;
        }
        // Close old file descriptor.
        char* conn_str = conn_h2a_2string(self);
        if (0 > close(self->fd))
        {
                console_log(console, ConsoleLogSevere, "Failed to close device %s", conn_str), free(conn_str);
                console_log(console, ConsoleLogSevere, "Cause: %s", strerror(errno));
                return false;
        } else {
                console_log(console, ConsoleLogSevere, "Successfully closed device %s", conn_str), free(conn_str);
                return true;
        }
}

bool conn_h2a_is_connected(struct conn_h2a* self)
{
        return self->fd >= 0;
}

struct action_protocol* conn_h2a_get_action(struct conn_h2a* self, bool is_blocking)
{
        abort();
}

void conn_h2a_puts(struct conn_h2a* self, const char* s)
{
        abort();
}

char* conn_h2a_gets(struct conn_h2a* self)
{
        if (!conn_h2a_is_connected(self))
                return nullptr;

        int n_fetched, n_consumed = 0;
        struct hidusb_request request = {0};
        uint8_t* buf = (uint8_t*) &request;
        bool is_valid = false;

        // Get a full packet before exiting.
        do {
                n_fetched = read(self->fd, &buf[n_consumed], sizeof(struct hidusb_request) - n_consumed);
                if (n_fetched > 0 && request.report_id == 0x1) {
                        is_valid = true;
                        n_consumed += n_fetched;
                }
        } while (is_valid && n_consumed < sizeof(struct hidusb_request));

        return n_consumed > 0 ? strdup(request.buf) : nullptr;
}

void conn_h2a_put_directory_structure(struct conn_h2a* self, struct directory* src)
{
        abort();
}

void conn_h2a_get_directory_structure(struct conn_h2a* self, struct directory* dst)
{
        abort();
}

void conn_h2a_put_file(struct conn_h2a* self, struct file* src)
{
        abort();
}

void conn_h2a_get_file(struct conn_h2a* self, struct file* dst)
{
        abort();
}

void conn_h2a_put_audio_player_state(struct conn_h2a* self, struct audioplayer_state* aps)
{
        abort();
}

void conn_h2a_get_audio_player_state(struct conn_h2a* self, struct audioplayer_state* aps)
{
        abort();
}

char* conn_h2a_2string(struct conn_h2a* self)
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

/*
 * <conn_local> public
 */
struct conn_local* conn_local_create(struct console* console, struct filesystem* base)
{
        abort();
}

bool conn_local_init(struct conn_local* self, struct filesystem* base)
{
        abort();
}

void conn_local_free(struct conn_local* self)
{
        abort();
}

bool conn_local_connect_to(struct conn_local* self)
{
        abort();
}

bool conn_local_disconnect(struct conn_local* self)
{
        abort();
}

bool conn_local_is_connected(struct conn_local* self)
{
        abort();
}

struct action_protocol* conn_local_get_action(struct conn_local* self, bool is_blocking)
{
        abort();
}

void conn_local_puts(struct conn_local* self, const char* s)
{
        abort();
}

char* conn_local_gets(struct conn_local* self)
{
        abort();
}

void conn_local_put_directory_structure(struct conn_local* self, struct directory* src)
{
        abort();
}

void conn_local_get_directory_structure(struct conn_local* self, struct directory* dst)
{
        abort();
}

void conn_local_put_file(struct conn_local* self, struct file* src)
{
        abort();
}

void conn_local_get_file(struct conn_local* self, struct file* dst)
{
        abort();
}

void conn_local_put_audio_player_state(struct conn_local* self, struct audioplayer_state* aps)
{
        abort();
}

void conn_local_get_audio_player_state(struct conn_local* self, struct audioplayer_state* aps)
{
        abort();
}

char* conn_local_2string(struct conn_local* self)
{
        abort();
}

#endif  // ARCH_X86_64
