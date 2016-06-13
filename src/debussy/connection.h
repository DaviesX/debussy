#ifndef CONNECTION_H_INCLUDED
#define CONNECTION_H_INCLUDED



/*
 * <connection> decl
 */
struct console;
struct action_protocol;
struct file;
struct directory;
struct audioplayer_state;
struct connection;

typedef bool                    (*f_Conn_Connect_To) (struct connection* self);
typedef bool                    (*f_Conn_Disconnect) (struct connection* self);
typedef struct action_protocol* (*f_Conn_Get_Action) (struct connection* self, bool is_blocking);
typedef void                    (*f_Conn_Puts) (struct connection* self, const char* s);
typedef char*                   (*f_Conn_Gets) (struct connection* self);
typedef void                    (*f_Conn_Put_Directory_Structure) (struct connection* self, struct directory* src);
typedef void                    (*f_Conn_Get_Directory_Structure) (struct connection* self, struct directory* dst);
typedef void                    (*f_Conn_Put_File) (struct connection* self, struct file* src);
typedef void                    (*f_Conn_Get_File) (struct connection* self, struct file* dst);
typedef void                    (*f_Conn_Put_Audio_Player_State) (struct connection* self, struct audioplayer_state* aps);
typedef void                    (*f_Conn_Get_Audio_Player_State) (struct connection* self, struct audioplayer_state* aps);

struct connection {
        struct console*                 console;
        f_Conn_Connect_To               f_connect_to;
        f_Conn_Disconnect               f_disconnect;
        f_Conn_Get_Action               f_get_action;
        f_Conn_Puts                     f_puts;
        f_Conn_Gets                     f_gets;
        f_Conn_Put_Directory_Structure  f_put_directory_structure;
        f_Conn_Get_Directory_Structure  f_get_directory_structure;
        f_Conn_Put_File                 f_put_file;
        f_Conn_Get_File                 f_get_file;
        f_Conn_Put_Audio_Player_State   f_put_audio_player_state;
        f_Conn_Get_Audio_Player_State   f_get_audio_player_state;
};

/*
 * <connection> public
 */
void                    conn_init(struct connection* self, struct console* console,
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
                                  f_Conn_Get_Audio_Player_State f_get_audio_player_state);
void                    conn_free(struct connection* self);
bool                    conn_connect_to(struct connection* self);
bool                    conn_disconnect(struct connection* self);
struct action_protocol* conn_get_action(struct connection* self, bool is_blocking);
void                    conn_puts(struct connection* self, const char* s);
char*                   conn_gets(struct connection* self);
void                    conn_put_directory_structure(struct connection* self, struct directory* src);
void                    conn_get_directory_structure(struct connection* self, struct directory* dst);
void                    conn_put_file(struct connection* self, struct file* src);
void                    conn_get_file(struct connection* self, struct file* dst);
void                    conn_put_audio_player_state(struct connection* self, struct audioplayer_state* aps);
void                    conn_get_audio_player_state(struct connection* self, struct audioplayer_state* aps);


#endif // CONNECTION_H_INCLUDED
