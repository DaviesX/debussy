#include <string.h>
#include <avr.h>
#include <actionprotocols.h>
#include <console.h>
#include <filesystem.h>
#include <audioplayer.h>
#include <connection.h>


/*
 * <connection> public
 */
void conn_init(struct connection* self, struct console* console,
               f_Conn_Connect_To f_connect_to,
               f_Conn_Disconnect f_disconnect,
               f_Conn_Get_Action f_get_action,
               f_Conn_Puts f_puts,
               f_Conn_Gets f_gets,
               f_Conn_Put_Directory_Structure f_put_directory_structure,
               f_Conn_Get_Directory_Structure f_get_directory_structure,
               f_Conn_Put_File f_put_file,
               f_Conn_Get_File f_get_file,
               f_Conn_Put_Audio_Player_State f_put_audio_player_state,
               f_Conn_Get_Audio_Player_State f_get_audio_player_state)
{
        memset(self, 0, sizeof(*self));
        self->console = console;
        self->f_connect_to = f_connect_to;
        self->f_disconnect = f_disconnect;
        self->f_get_action = f_get_action;
        self->f_puts = f_puts;
        self->f_gets = f_gets;
        self->f_put_directory_structure = f_put_directory_structure;
        self->f_get_directory_structure = f_get_directory_structure;
        self->f_put_file = f_put_file;
        self->f_get_file = f_get_file;
        self->f_put_audio_player_state = f_put_audio_player_state;
        self->f_get_audio_player_state = f_get_audio_player_state;
}

void conn_free(struct connection* self)
{
}

bool conn_connect_to(struct connection* self)
{
        return self->f_connect_to(self);
}

bool conn_disconnect(struct connection* self)
{
        return self->f_disconnect(self);
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
