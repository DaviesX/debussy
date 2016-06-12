#ifndef WINDOW_H_INCLUDED
#define WINDOW_H_INCLUDED

#include <console.h>

/*
 * <window> decl
 */
struct window_impl;
struct window {
        char*                   title;
        int                     w;
        int                     h;
        int*                    argc;
        char***                 argv;
        struct console*         console;
        struct window_impl*     pimpl;
};

/*
 * <window> public
 */
void window_init(struct window* self, int* argc, char*** argv);
void window_free(struct window* self);
void window_set_title(struct window* self, const char* title);
void window_set_size(struct window* self, const int w, const int h);
void window_update(struct window* self);
void window_run(struct window* self);


#endif // WINDOW_H_INCLUDED
