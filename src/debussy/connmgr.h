#ifndef CONNMGR_H_INCLUDED
#define CONNMGR_H_INCLUDED

/*
 * <conn_manager> decl
 */
struct connection;
struct filesystem;
struct console;
struct conn_manager {
        struct connection**     conns;
        size_t                  num_conns;
};

/*
 * <conn_manager> public
 */
void                    connmgr_init(struct conn_manager* self);
void                    connmgr_free(struct conn_manager* self);
size_t                  connmgr_size(const struct conn_manager* self);
bool                    connmgr_add_scanned_avr_connections(struct conn_manager* self, struct console* console);
bool                    connmgr_add_local_connection(struct conn_manager* self, struct filesystem* base, struct console* console);
void                    connmgr_remove_connection(struct conn_manager* self, const char* id);
struct connection*      connmgr_get_connection(const struct conn_manager* self, const char* id);
struct connection*      connmgr_get_connection_at(const struct conn_manager* self, size_t i);
const struct connection** connmgr_get_all_connections(const struct conn_manager* self);
char**                  connmgr_2strings(const struct conn_manager* self);


#endif // CONNMGR_H_INCLUDED
