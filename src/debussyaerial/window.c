#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <aerial.h>
#include <window.h>


#define DEFAULT_WINDOW_TITLE    "debussy - Aerial"
#define DEFAULT_WINDOW_WIDTH    800
#define DEFAULT_WINDOW_HEIGHT   600

/*
 * <window> public
 */
void window_init(struct window* self, int* argc, char*** argv)
{
        self->win_widget = nullptr;
        self->argc = argc;
        self->argv = argv;

        window_set_size(self, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
        window_set_title(self, DEFAULT_WINDOW_TITLE);
}

void window_free(struct window* self)
{
}

void window_set_title(struct window* self, const char* title)
{
        free(self->title);
        self->title = strdup(title);
}

void window_set_size(struct window* self, const int w, const int h)
{
        self->w = w;
        self->h = h;
}

void window_update(struct window* self)
{
        if (self->win_widget != NULL) {
                gtk_window_set_title(GTK_WINDOW(self->win_widget), self->title);
                gtk_widget_set_size_request(GTK_WIDGET(self->win_widget), self->w, self->h);
        }
}

void window_run(struct window* self)
{
        // Initialize GTK+.
        g_log_set_handler("Gtk", G_LOG_LEVEL_WARNING, (GLogFunc) gtk_false, NULL);
        gtk_init(self->argc, self->argv);
        g_log_set_handler("Gtk", G_LOG_LEVEL_WARNING, g_log_default_handler, NULL);

        // Create the main window.
        self->win_widget = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        window_update(self);
        gtk_window_set_position(GTK_WINDOW(self->win_widget), GTK_WIN_POS_CENTER);
        gtk_container_set_border_width(GTK_CONTAINER (self->win_widget), 8);
        gtk_widget_realize(self->win_widget);

        // Install signals.
        g_signal_connect(self->win_widget, "destroy", gtk_main_quit, NULL);

        // Enter the main loop.
        gtk_widget_show_all(self->win_widget);
        gtk_main();
}
