#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <aerial.h>
#include <console.h>
#include <usb.h>
#include <window.h>


#define DEFAULT_WINDOW_TITLE    AERIAL_VERSION_STRING
#define DEFAULT_WINDOW_WIDTH    800
#define DEFAULT_WINDOW_HEIGHT   600

struct window_impl {
        struct window*          public_ref;
        struct usb              usb;
        struct usb_connection*  conns;
        int                     n_conns;
};

/*
 * <window> private
 */
static void __window_impl_init(struct window_impl* self, struct window* public_ref)
{
        memset(self, 0, sizeof(*self));
        self->public_ref = public_ref;
        usb_init(&self->usb);
}

static void __window_impl_free(struct window_impl* self)
{
        usb_free(&self->usb);
        free(self->conns);
        memset(self, 0, sizeof(*self));
}

static gboolean __window_impl_on_scan_connections(GtkMenuItem* menuitem, gpointer user_data)
{
        struct window_impl* self = (struct window_impl*) user_data;

        free(self->conns);
        self->conns = usb_scan_connections(&self->usb, &self->n_conns, self->public_ref->console);
}

static gboolean __window_impl_on_connect2avr(GtkMenuItem* menuitem, gpointer user_data)
{
}


/*
 * <window> public
 */
void window_init(struct window* self, int* argc, char*** argv)
{
        memset(self, 0, sizeof(*self));
        self->win_widget = nullptr;
        self->argc = argc;
        self->argv = argv;
        self->console = stdconsole_create();

        self->pimpl = malloc(sizeof(struct window_impl));
        __window_impl_init(self->pimpl, self);

        window_set_size(self, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
        window_set_title(self, DEFAULT_WINDOW_TITLE);
}

void window_free(struct window* self)
{
        console_free(self->console);
        __window_impl_free(self->pimpl);

        free(self->pimpl);
        free(self->console);
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

void window_push_status(struct window* self, const char* status)
{
        if (self->status_bar) {
                guint id = gtk_statusbar_get_context_id(self->status_bar, AERIAL_VERSION_STRING);
                gtk_statusbar_push(self->status_bar, id, status);
        }
}

void window_pop_status(struct window* self)
{
}

void window_update(struct window* self)
{
        if (self->win_widget != NULL) {
                gtk_window_set_title(GTK_WINDOW(self->win_widget), self->title);
                gtk_widget_set_size_request(GTK_WIDGET(self->win_widget), self->w, self->h);
        }
}

GtkBuilder* __window_builder_load(const struct window* self, const char* filename)
{
        GtkBuilder* builder = nullptr;
        if (!(builder = gtk_builder_new())) {
                console_log(self->console, ConsoleLogSevere, "Cannot create gtk-glade builder");
                return nullptr;
        }
        GError* error = nullptr;
        if (!(gtk_builder_add_from_file(builder, filename, &error))) {
                if (error) {
                        console_log(self->console, ConsoleLogSevere,
                                    "Builder fail to load: %s\n%s", filename, error->message);
                        g_free(error);
                } else {
                        console_log(self->console, ConsoleLogSevere,
                                    "Builder fail to load: %s, unknown error", filename);
                }
                return nullptr;
        }
        return builder;
}

void window_run(struct window* self)
{
        console_log(self->console, ConsoleLogNormal, "Initializing GTK+...");
        g_log_set_handler("Gtk", G_LOG_LEVEL_WARNING, (GLogFunc) gtk_false, NULL);
        gtk_init(self->argc, self->argv);
        g_log_set_handler("Gtk", G_LOG_LEVEL_WARNING, g_log_default_handler, NULL);

        // Create the main window from builder.
        GtkBuilder* builder = __window_builder_load(self, "data/debussy-aerial.glade");
        if (builder == nullptr) {
                console_log(self->console, ConsoleLogSevere, "Failed to build aerial UI. Use fallback window.");
                self->win_widget = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        } else {
                console_log(self->console, ConsoleLogSevere, "Window has been loaded.");
                self->win_widget = (GtkWidget*) gtk_builder_get_object(builder, "win-main-frame");
                self->status_bar = (GtkStatusbar*) gtk_builder_get_object(builder, "sb-main");
                GtkTextView* tv_console = (GtkTextView*) gtk_builder_get_object(builder, "tv-console");
                if (tv_console == nullptr) {
                        console_log(self->console, ConsoleLogSevere, "Cannot load gtk console widget. Use fallback console.");
                } else {
                        console_log(self->console, ConsoleLogNormal, "Connecting to gtk console...");
                        console_free(self->console);
                        self->console = gtkconsole_create(tv_console);
                }

                // Load menus and connect signals.
                GtkMenuItem* mi_scan_conn = (GtkMenuItem*) gtk_builder_get_object(builder, "mi-scan-connection");
                if (mi_scan_conn == nullptr) {
                        console_log(self->console, ConsoleLogSevere, "Cannot load gtk scan connection menu item.");
                } else {
                        g_signal_connect(G_OBJECT(mi_scan_conn), "activate",
                                         G_CALLBACK(__window_impl_on_scan_connections), self->pimpl);
                }
                GtkMenuItem* mi_conn2avr = (GtkMenuItem*) gtk_builder_get_object(builder, "mi-connect2avr");
                if (mi_conn2avr == nullptr) {
                        console_log(self->console, ConsoleLogSevere, "Cannot load gtk scan connection menu item.");
                } else {
                        g_signal_connect(G_OBJECT(mi_conn2avr), "activate",
                                         G_CALLBACK(__window_impl_on_connect2avr), self->pimpl);
                }
        }

        window_update(self);
        gtk_window_set_position(GTK_WINDOW(self->win_widget), GTK_WIN_POS_CENTER);
        gtk_widget_realize(self->win_widget);

        window_push_status(self, AERIAL_VERSION_STRING);

        // Install signals.
        g_signal_connect(self->win_widget, "destroy", gtk_main_quit, NULL);

        // Enter the main loop.
        gtk_widget_show_all(self->win_widget);
        gtk_main();
}
