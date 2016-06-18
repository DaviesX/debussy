#ifndef SET_H_INCLUDED
#define SET_H_INCLUDED


#define setptr(T)          struct __set

#define int_hash(_x)    (0x9e370001*(_x))

#define NIL     -1

/*
 * <set> decl
 */
#define set_templ(T, D)                                                                                    \
struct {                                                                                                   \
        struct {                                                                                           \
                T       value;                                                                             \
                int     next;                                                                              \
        } *chunk;                                                                                          \
        int*    bins;                                                                                      \
        int     num_bin;                                                                                   \
        int     num_elm;                                                                                   \
        int*    recycled;                                                                                  \
        int     num_recyc;                                                                                 \
        D*      data;                                                                                      \
}

/*
 * <set> private
 */
#define __set_pos(__i, __size)              ((__i) & ((__size) - 1))

#define __set_add_value_at(_self, _hash, _p)                                                               \
{                                                                                                          \
        (_self)->chunk[_p].next = NIL;                                                                     \
                                                                                                           \
        int _bin = __set_pos(_hash, (_self)->num_bin);                                                     \
        if ((_self)->bins[_bin] == NIL) {                                                                  \
                (_self)->bins[_bin] = (_p);                                                                \
        } else {                                                                                           \
                int _list = (_self)->bins[_bin];                                                           \
                while ((_self)->chunk[_list].next != NIL) {                                                \
                        _list = (_self)->chunk[_list].next;                                                \
                }                                                                                          \
                (_self)->chunk[_list].next = (_p);                                                         \
        }                                                                                                  \
}

#define __set_ensure_ratio(_self, f_Hash)                                                                  \
{                                                                                                          \
        if (((_self)->num_bin << 1) < (_self)->num_elm + 1) {                                              \
                (_self)->num_bin <<= 1;                                                                    \
                (_self)->bins = realloc((_self)->bins, (_self)->num_bin*sizeof(*(_self)->bins));           \
                int _i;                                                                                    \
                for (_i = 0; _i < (_self)->num_bin; _i ++) {                                               \
                        (_self)->bins[_i] = NIL;                                                           \
                }                                                                                          \
                for (_i = 0; _i < (_self)->num_elm; _i ++) {                                               \
                        __set_add_value_at(_self, f_Hash((_self)->chunk[_i].value), _i);                   \
                }                                                                                          \
        }                                                                                                  \
}

#define __set_alloc_node(_self, _node)                                                                     \
{                                                                                                          \
        if ((_self)->num_recyc != 0) {                                                                     \
                (_node) = (_self)->recycled[-- (_self)->num_recyc];                                        \
        } else {                                                                                           \
                (_self)->chunk = realloc((_self)->chunk, ((_self)->num_elm + 1)*sizeof(*(_self)->chunk));  \
                (_node) = (_self)->num_elm;                                                                \
        }                                                                                                  \
}

#define __set_free_node(_self, _node)                                                                      \
{                                                                                                          \
        (_self)->recycled = realloc((_self)->recycled, ((_self)->num_recyc + 1)*sizeof(*(_self)->recycled));\
        (_self)->recycled[(_self)->num_recyc ++] = (_node);                                                \
}

/*
 * <set> public
 */
static inline int log2_int_c(int x)
{
        uint32_t shift = x;
        int i_log = 0;
        while ( shift >>= 1 ) {
                i_log ++;
        }
        return i_log + (x != 1 << i_log);
}

#define set_init(_self, _init_count, _data)                                                                \
{                                                                                                          \
        int _scale = (_init_count) > 1 ? (_init_count)*2 : 1;                                              \
        int _perf_power = log2_int_c(_scale);                                                              \
        (_self)->num_bin = 1 << _perf_power;                                                               \
        (_self)->num_elm = 0;                                                                              \
                                                                                                           \
        (_self)->chunk = malloc(sizeof(*(_self)->chunk)*(_self)->num_elm + 1);                             \
        (_self)->bins = malloc(sizeof(int)*(_self)->num_bin);                                              \
        int _i;                                                                                            \
        for (_i = 0; _i < (_self)->num_bin; _i ++) {                                                       \
                (_self)->bins[_i] = NIL;                                                                   \
        }                                                                                                  \
                                                                                                           \
        (_self)->recycled = malloc(sizeof(int)*1);                                                         \
        (_self)->num_recyc = 0;                                                                            \
                                                                                                           \
        (_self)->data = (_data);                                                                           \
}

#define set_reset_data(_self, _data)                                                                       \
{                                                                                                          \
        (_self)->data = (_data);                                                                           \
}

#define set_free(_self)                                                                                    \
{                                                                                                          \
        free((_self)->chunk);                                                                              \
        free((_self)->bins);                                                                               \
}

#define set_flush(_self)                                                                                   \
{                                                                                                          \
        (_self)->num_bin = 1;                                                                              \
        (_self)->bins[0] = NIL;                                                                            \
        (_self)->num_recyc = 0;                                                                            \
        (_self)->num_elm = 0;                                                                              \
}

#define set_size(_self)                                                                                    \
        ((_self)->num_elm)

#define __set_put_location_to(_self, _value, _iter, _dest)                                                 \
{                                                                                                          \
        (_self)->chunk[(_iter)->pos].value = (_value);                                                     \
        (_self)->chunk[(_iter)->pos].next = NIL;                                                           \
        *(_dest) = (_iter)->pos;                                                                           \
}

#define set_add(_self, _value, f_Hash, f_Is_Equal, _iter)                                                  \
{                                                                                                          \
        __set_ensure_ratio(_self, f_Hash);                                                                 \
        (_iter)->bin = __set_pos(f_Hash(_value), (_self)->num_bin);                                        \
        (_iter)->pos = (_self)->num_elm;                                                                   \
                                                                                                           \
        int _list = (_self)->bins[(_iter)->bin];                                                           \
        if (_list == NIL) {                                                                                \
                (_iter)->last = NIL;                                                                       \
                __set_alloc_node(_self, (_iter)->pos);                                                     \
                __set_put_location_to(_self, _value, _iter, &(_self)->bins[(_iter)->bin]);                 \
                (_self)->num_elm ++;                                                                       \
        } else {                                                                                           \
                while (!f_Is_Equal(&(_value), &(_self)->chunk[_list].value, (_self)->data)) {              \
                        if ((_self)->chunk[_list].next == NIL) {                                           \
                                (_iter)->last = _list;                                                     \
                                __set_alloc_node(_self, (_iter)->pos);                                     \
                                __set_put_location_to(_self, _value, _iter, &(_self)->chunk[_list].next);  \
                                (_self)->num_elm ++;                                                       \
                                break;                                                                     \
                        }                                                                                  \
                        _list = (_self)->chunk[_list].next;                                                \
                }                                                                                          \
        }                                                                                                  \
}

#define set_find(_self, _value, f_Hash, f_Is_Equal, _iter)                                                 \
{                                                                                                          \
        (_iter)->pos = (_self)->num_elm;                                                                   \
        (_iter)->bin = __set_pos(f_Hash(_value), (_self)->num_bin);                                        \
                                                                                                           \
        int _prev = NIL;                                                                                   \
        int _list = (_self)->bins[(_iter)->bin];                                                           \
                                                                                                           \
        while (_list != NIL) {                                                                             \
                if (f_Is_Equal(&(_value), &(_self)->chunk[_list].value, (_self)->data)) {                  \
                        (_iter)->pos = _list;                                                              \
                        (_iter)->last = _prev;                                                             \
                        break;                                                                             \
                }                                                                                          \
                _prev = _list;                                                                             \
                _list = (_self)->chunk[_list].next;                                                        \
        }                                                                                                  \
}

#define set_remove_at(_self, _iter)                                                                        \
{                                                                                                          \
        if ((_iter)->last == NIL) {                                                                        \
                (_self)->bins[(_iter)->bin] = NIL;                                                         \
        } else {                                                                                           \
                (_self)->chunk[(_iter)->last].next = (_self)->chunk[(_iter)->pos].next;                    \
        }                                                                                                  \
        __set_free_node(_self, (_iter)->pos);                                                              \
        (_self)->num_elm --;                                                                               \
}


/*
 * <deque_iterator> decl
 */
#define set_iter_templ(T)                                                                                  \
struct {                                                                                                   \
        int     pos;                                                                                       \
        int     bin;                                                                                       \
        int     last;                                                                                      \
}
/*
 * <deque_iterator> public
 */
#define set_iter_begin(_self, _set)                                                                        \
{                                                                                                          \
        (_self)->pos = 0;                                                                                  \
}

#define set_iter_next(_self, _set)                                                                         \
{                                                                                                          \
        (_self)->pos ++;                                                                                   \
}

#define set_iter_has_next(_self, _set)                                                                     \
        ((_self)->pos < (_set)->num_elm)

#define set_iter_deref(_self, _set)                                                                        \
        ((_set)->chunk[(_self)->pos].value)

#define set_for_each(_self, _elm, CODE)                                                                    \
{                                                                                                          \
        set_iter_templ(typeof(*(_self)->values)) _iterator;                                                \
        set_iter_begin(&_iterator, _self);                                                                 \
        while (set_iter_has_next(&_iterator, _self)) {                                                     \
                (_elm) = set_iter_deref(&_iterator, _self);                                                \
                CODE;                                                                                      \
                set_iter_next(&_iterator, _self);                                                          \
        }                                                                                                  \
}


#endif	// SET_H_INCLUDED
