#ifndef CONNECTION_H_INCLUDED
#define CONNECTION_H_INCLUDED

#include <types.h>
#include <string.h>

/*
 * <connection> decl
 */
enum ConnectionType {
        ConnectionAvr2Host,
        ConnectionHost2Avr,
        ConnectionHostLocal,
};

struct console;
struct action_protocol;
struct filesystem;
struct file;
struct directory;
struct audioplayer_state;
struct connection;

typedef void                    (*f_Conn_Free) (struct connection* self);
typedef bool                    (*f_Conn_Connect) (struct connection* self);
typedef bool                    (*f_Conn_Disconnect) (struct connection* self);
typedef bool                    (*f_Conn_Is_Connected) (const struct connection* self);
typedef struct action_protocol* (*f_Conn_Get_Action) (struct connection* self, bool is_blocking);
typedef void                    (*f_Conn_Puts) (struct connection* self, const char* s);
typedef const char*             (*f_Conn_Gets) (struct connection* self);
typedef void                    (*f_Conn_Put_Path) (struct connection* self, const char* path, bool has_next);
typedef const char*             (*f_Conn_Get_Path) (struct connection* self, bool* has_next);
typedef void                    (*f_Conn_Put_File) (struct connection* self, struct file* src);
typedef void                    (*f_Conn_Get_File) (struct connection* self, struct file* dst);
typedef void                    (*f_Conn_Put_Audio_Player_State) (struct connection* self, struct audioplayer_state* aps);
typedef void                    (*f_Conn_Get_Audio_Player_State) (struct connection* self, struct audioplayer_state* aps);
typedef const char*             (*f_Conn_2string) (const struct connection* self);

struct connection {
        char*                           id;
        uint8_t                         type;
        struct console*                 console;
        f_Conn_Free                     f_free;
        f_Conn_Connect                  f_connect;
        f_Conn_Disconnect               f_disconnect;
        f_Conn_Is_Connected             f_is_connected;
        f_Conn_Get_Action               f_get_action;
        f_Conn_Puts                     f_puts;
        f_Conn_Gets                     f_gets;
        f_Conn_Put_Path                 f_put_path;
        f_Conn_Get_Path                 f_get_path;
        f_Conn_Put_File                 f_put_file;
        f_Conn_Get_File                 f_get_file;
        f_Conn_Put_Audio_Player_State   f_put_audio_player_state;
        f_Conn_Get_Audio_Player_State   f_get_audio_player_state;
        f_Conn_2string                  f_2string;
};

/*
 * <connection> public
 */
void                    conn_init(struct connection* self, struct console* console, const char* id, uint8_t type,
                                  f_Conn_Free f_free,
                                  f_Conn_Connect f_connect,
                                  f_Conn_Disconnect f_disconnect,
                                  f_Conn_Is_Connected f_is_connected,
                                  f_Conn_Get_Action f_get_action,
                                  f_Conn_Puts f_puts,
                                  f_Conn_Gets f_gets,
                                  f_Conn_Put_Path f_put_path,
                                  f_Conn_Get_Path f_get_path,
                                  f_Conn_Put_File f_put_file,
                                  f_Conn_Get_File f_get_file,
                                  f_Conn_Put_Audio_Player_State f_put_audio_player_state,
                                  f_Conn_Get_Audio_Player_State f_get_audio_player_state,
                                  f_Conn_2string f_2string);
void                    conn_free(struct connection* self);
#define                 conn_is_equal(__self, __id)   (!strcmp((__self)->id, __id))
#define                 conn_is_of_type(__self, __type)  ((__self)->type == (__type))
bool                    conn_conect(struct connection* self);
bool                    conn_disconnect(struct connection* self);
bool                    conn_is_connected(const struct connection* self);
struct action_protocol* conn_get_action(struct connection* self, bool is_blocking);
void                    conn_puts(struct connection* self, const char* s);
const char*             conn_gets(struct connection* self);
void                    conn_put_path(struct connection* self, const char* path, bool has_next);
const char*             conn_get_path(struct connection* self, bool* has_next);
void                    conn_put_file(struct connection* self, struct file* src);
void                    conn_get_file(struct connection* self, struct file* dst);
void                    conn_put_audio_player_state(struct connection* self, struct audioplayer_state* aps);
void                    conn_get_audio_player_state(struct connection* self, struct audioplayer_state* aps);
const char*             conn_2string(const struct connection* self);

#ifndef ARCH_X86_64
/*
 * <conn_a2h> decl
 * avr-to-host connection.
 */
struct conn_a2h {
        struct connection __parent;
};

/*
 * <conn_a2h> public
 */
struct conn_a2h*        conn_a2h_create();
void                    conn_a2h_init(struct conn_a2h* self);
void                    conn_a2h_free(struct conn_a2h* self);
bool                    conn_a2h_connect(struct conn_a2h* self);
bool                    conn_a2h_disconnect(struct conn_a2h* self);
bool                    conn_a2h_is_connected(const struct conn_a2h* self);
struct action_protocol* conn_a2h_get_action(struct conn_a2h* self, bool is_blocking);
void                    conn_a2h_puts(struct conn_a2h* self, const char* s);
const char*             conn_a2h_gets(struct conn_a2h* self);
void                    conn_a2h_put_path(struct conn_a2h* self, const char* path, bool has_next);
const char*             conn_a2h_get_path(struct conn_a2h* self, bool* has_next);
void                    conn_a2h_put_file(struct conn_a2h* self, struct file* src);
void                    conn_a2h_get_file(struct conn_a2h* self, struct file* dst);
void                    conn_a2h_put_audio_player_state(struct conn_a2h* self, struct audioplayer_state* aps);
void                    conn_a2h_get_audio_player_state(struct conn_a2h* self, struct audioplayer_state* aps);
const char*             conn_a2h_2string(const struct conn_a2h* self);

/*
 * <conn_a2h> test cases
 */
void conn_a2h_puts_test();

#else

/*
 * <conn_h2a> decl
 * host-to-avr connection.
 */
struct conn_h2a {
        struct connection __parent;
        int               fd;
        char*             vender_id;
        char*             name;
        char*             manufacturer;
        char*             dev_node_path;
};

/*
 * <conn_h2a> public
 */
struct conn_h2a*        conn_h2a_create(struct console* console,
                                        const char* vender_id,
                                        const char* name,
                                        const char* manufacturer,
                                        const char* dev_node_path);
void                    conn_h2a_init(struct conn_h2a* self, struct console* console,
                                      const char* vender_id,
                                      const char* name,
                                      const char* manufacturer,
                                      const char* dev_node_path);
void                    conn_h2a_free(struct conn_h2a* self);
bool                    conn_h2a_connect(struct conn_h2a* self);
bool                    conn_h2a_disconnect(struct conn_h2a* self);
bool                    conn_h2a_is_connected(const struct conn_h2a* self);
struct action_protocol* conn_h2a_get_action(struct conn_h2a* self, bool is_blocking);
void                    conn_h2a_puts(struct conn_h2a* self, const char* s);
const char*             conn_h2a_gets(struct conn_h2a* self);
void                    conn_h2a_put_path(struct conn_h2a* self, const char* path, bool has_next);
const char*             conn_h2a_get_path(struct conn_h2a* self, bool* has_next);
void                    conn_h2a_put_file(struct conn_h2a* self, struct file* src);
void                    conn_h2a_get_file(struct conn_h2a* self, struct file* dst);
void                    conn_h2a_put_audio_player_state(struct conn_h2a* self, struct audioplayer_state* aps);
void                    conn_h2a_get_audio_player_state(struct conn_h2a* self, struct audioplayer_state* aps);
const char*             conn_h2a_2string(const struct conn_h2a* self);


/*
 * <conn_local> decl
 * host's local connection.
 */
struct conn_local {
        struct connection   __parent;
        struct filesystem*  base;

        char**              cached_file_list;
        int                 n_cached;
        int                 n_fetched;

        bool                is_connected;
};

/*
 * <conn_local> public
 */
struct conn_local*      conn_local_create(struct console* console, struct filesystem* base);
bool                    conn_local_init(struct conn_local* self, struct console* console, struct filesystem* base);
void                    conn_local_free(struct conn_local* self);
bool                    conn_local_connect(struct conn_local* self);
bool                    conn_local_disconnect(struct conn_local* self);
bool                    conn_local_is_connected(const struct conn_local* self);
struct action_protocol* conn_local_get_action(struct conn_local* self, bool is_blocking);
void                    conn_local_puts(struct conn_local* self, const char* s);
const char*             conn_local_gets(struct conn_local* self);
void                    conn_local_put_path(struct conn_local* self, const char* path, bool has_next);
const char*             conn_local_get_path(struct conn_local* self, bool* has_next);
void                    conn_local_put_file(struct conn_local* self, struct file* src);
void                    conn_local_get_file(struct conn_local* self, struct file* dst);
void                    conn_local_put_audio_player_state(struct conn_local* self, struct audioplayer_state* aps);
void                    conn_local_get_audio_player_state(struct conn_local* self, struct audioplayer_state* aps);
const char*             conn_local_2string(const struct conn_local* self);

/*
 * <conn_local> test cases.
 */
void conn_local_test_get_path();


#endif // ARCH_X86_64


#endif // CONNECTION_H_INCLUDED
