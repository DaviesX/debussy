#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <aerial.h>
#include <console.h>
#include <connection.h>
#include <connmgr.h>
#include <window.h>


#define DEFAULT_WINDOW_TITLE    AERIAL_VERSION_STRING
#define DEFAULT_WINDOW_WIDTH    800
#define DEFAULT_WINDOW_HEIGHT   600

struct window_impl {
        struct window*          public_ref;
        GtkWindow*              win_frame;
        GtkStatusbar*           sb_status;
        GtkDialog*              dl_helpabout;
        GtkDialog*              dl_connection;
        GtkComboBoxText*        cb_selected;
        GtkButton*              bt_conn_confirm;
        GtkButton*              bt_conn_cancel;
        struct conn_manager     conn_mgr;
        struct connection*      curr_conn;
};

/*
 * <window> private
 */
/******************* APIs *******************/
static void __window_impl_init(struct window_impl* self, struct window* public_ref)
{
        memset(self, 0, sizeof(*self));
        self->public_ref = public_ref;
        connmgr_init(&self->conn_mgr);
}

static void __window_impl_free(struct window_impl* self)
{
        connmgr_free(&self->conn_mgr);
        memset(self, 0, sizeof(*self));
}

static void __window_impl_set_ui(struct window_impl* self,
                                 GtkWindow* win_frame,
                                 GtkStatusbar* sb_status,
                                 GtkDialog* dl_helpabout,
                                 GtkDialog* dl_connection,
                                 GtkComboBoxText* cb_selected,
                                 GtkButton* bt_conn_confirm,
                                 GtkButton* bt_conn_cancel)
{
        self->win_frame = win_frame,
        self->sb_status = sb_status;
        self->dl_helpabout = dl_helpabout;
        self->dl_connection = dl_connection;
        self->cb_selected = cb_selected;
        self->bt_conn_confirm = bt_conn_confirm;
        self->bt_conn_cancel = bt_conn_cancel;
}
/******************* APIs *******************/

/******************* GUI utils *******************/
static void __window_impl_push_status(struct window_impl* self, const char* status)
{
        if (self->sb_status) {
                guint id = gtk_statusbar_get_context_id(self->sb_status, AERIAL_VERSION_STRING);
                gtk_statusbar_push(self->sb_status, id, status);
        }
}

static void __window_impl_pop_status(struct window_impl* self)
{
        if (self->sb_status) {
                guint id = gtk_statusbar_get_context_id(self->sb_status, AERIAL_VERSION_STRING);
                gtk_statusbar_pop(self->sb_status, id);
        }
}

static void __window_impl_update(struct window_impl* self, const char* title, int w, int h)
{
        if (self->win_frame) {
                gtk_window_set_title(GTK_WINDOW(self->win_frame), title);
                gtk_widget_set_size_request(GTK_WIDGET(self->win_frame), w, h);
        }
}

static int __window_impl_show_message_box(const char* title, const char* message, const GtkMessageType type, GtkWindow* parent)
{
        GtkDialogFlags flags;
        GtkButtonsType buttons;
        if (type == GTK_MESSAGE_QUESTION) {
                flags = GTK_DIALOG_MODAL;
                buttons = GTK_BUTTONS_YES_NO;
        } else {
                flags = GTK_DIALOG_DESTROY_WITH_PARENT;
                buttons = GTK_BUTTONS_OK;
        }
        GtkWidget* dialog = gtk_message_dialog_new(parent, flags,
                                                   type, buttons,
                                                   "%s", message);
        gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);
        gtk_window_set_title(GTK_WINDOW(dialog), title);
        gtk_window_set_modal(GTK_WINDOW(dialog), false);
        gtk_window_set_transient_for(GTK_WINDOW(dialog), parent);
        gtk_widget_show_all(dialog);
        if (type == GTK_MESSAGE_QUESTION) {
                gint ans = gtk_dialog_run(GTK_DIALOG (dialog));
                gtk_widget_destroy(dialog);
                return ans;
        } else {
                g_signal_connect_swapped(dialog, "response",
                                         G_CALLBACK (gtk_widget_destroy),
                                         dialog);
                return 0;
        }
}
/******************* GUI utils *******************/

/******************* Callback controllers *******************/
// Handling connections.
static void __window_impl_on_scan_connections(GtkMenuItem* menuitem, gpointer user_data)
{
        struct window_impl* self = (struct window_impl*) user_data;
        // Refresh connections.
        connmgr_add_scanned_avr_connections(&self->conn_mgr, self->public_ref->console);
}

static void __window_impl_on_conn_confirm(GtkButton* button, gpointer user_data)
{
        struct window_impl* self = (struct window_impl*) user_data;
        if (button == self->bt_conn_confirm) {
                struct connection* conn = connmgr_get_connection(&self->conn_mgr,
                                              gtk_combo_box_get_active_id(GTK_COMBO_BOX(self->cb_selected)));
                if (conn) {
                        char msg[256];
                        char* conn_str = conn_2string(conn);

                        if (!conn_connect_to(conn)) {
                                sprintf(msg, "Connection %s cannot be established", conn_str), free(conn_str);
                                __window_impl_show_message_box(AERIAL_VERSION_STRING, msg,
                                                               GTK_MESSAGE_ERROR, self->win_frame);
                        } else {
                                // Connected to the device successfully.
                                if (self->curr_conn) {
                                        // Disconnect the previous device.
                                        conn_disconnect(self->curr_conn);
                                }
                                self->curr_conn = conn;
                                sprintf(msg, "Has been connected to %s", conn_str), free(conn_str);
                                __window_impl_show_message_box(AERIAL_VERSION_STRING, msg,
                                                               GTK_MESSAGE_INFO, self->win_frame);
                                gtk_widget_hide(GTK_WIDGET(self->dl_connection));
                        }
                } else {
                        __window_impl_show_message_box(AERIAL_VERSION_STRING, "Invalid connection",
                                                       GTK_MESSAGE_ERROR, self->win_frame);
                }
        } else {
                gtk_widget_hide(GTK_WIDGET(self->dl_connection));
        }
}

static void __window_impl_on_connect2dev(GtkMenuItem* menuitem, gpointer user_data)
{
        struct window_impl* self = (struct window_impl*) user_data;

        // Refresh connections.
        __window_impl_on_scan_connections(menuitem, user_data);

        // Initialize selection box.
        char** texts = connmgr_2strings(&self->conn_mgr);
        const struct connection** conns = connmgr_get_all_connections(&self->conn_mgr);
        gtk_combo_box_text_remove_all(self->cb_selected);
        int i;
        for (i = 0; i < connmgr_size(&self->conn_mgr); i ++) {
                gtk_combo_box_text_insert(self->cb_selected, -1, conns[i]->id, texts[i]);
        }

        // Run selection dialog.
        g_signal_connect(self->bt_conn_confirm, "clicked", G_CALLBACK(__window_impl_on_conn_confirm), self);
        g_signal_connect(self->bt_conn_cancel, "clicked", G_CALLBACK(__window_impl_on_conn_confirm), self);
        gtk_widget_show(GTK_WIDGET(self->dl_connection));

        // Clean up texts.
        for (i = 0; i < connmgr_size(&self->conn_mgr); i ++) {
                free(texts[i]);
        }
        free(texts);
}

// Help about.
static void __window_impl_on_help_about(GtkMenuItem* menuitem, gpointer user_data)
{
        struct window_impl* self = (struct window_impl*) user_data;
        gtk_dialog_run(self->dl_helpabout);
}

// Device access.
static gboolean __window_impl_fetch_device_console(gpointer user_data)
{
        struct window_impl* self = (struct window_impl*) user_data;
        char* msg = self->curr_conn != nullptr ? conn_gets(self->curr_conn) : nullptr;
        if (msg != nullptr)
                console_log(self->public_ref->console, ConsoleLogNormal, "device: %s", msg), free(msg);
        return TRUE;
}

static gboolean __window_impl_capture_device(GtkButton* button, gpointer user_data)
{
        // struct window_impl* self = (struct window_impl*) user_data;
        return TRUE;
}
/******************* Callback controllers *******************/

/*
 * <window> public
 */
void window_init(struct window* self, int* argc, char*** argv)
{
        memset(self, 0, sizeof(*self));
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

void window_update(struct window* self)
{
        __window_impl_update(self->pimpl, self->title, self->w, self->h);
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
        GtkWindow* win_frame = nullptr;
        GtkBuilder* builder = __window_builder_load(self, "data/debussy-aerial.glade");
        if (builder == nullptr) {
                console_log(self->console, ConsoleLogSevere, "Failed to build aerial UI. Use fallback window.");
                win_frame = (GtkWindow*) gtk_window_new(GTK_WINDOW_TOPLEVEL);
        } else {
                console_log(self->console, ConsoleLogSevere, "Window has been loaded.");
                win_frame = (GtkWindow*) gtk_builder_get_object(builder, "win-main-frame");
                GtkStatusbar* sb_status = (GtkStatusbar*) gtk_builder_get_object(builder, "sb-main");
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
                        console_log(self->console, ConsoleLogSevere, "Cannot load scan connection menu item.");
                } else {
                        g_signal_connect(G_OBJECT(mi_scan_conn), "activate",
                                         G_CALLBACK(__window_impl_on_scan_connections), self->pimpl);
                }
                GtkMenuItem* mi_conn2dev = (GtkMenuItem*) gtk_builder_get_object(builder, "mi-connect2device");
                if (mi_conn2dev == nullptr) {
                        console_log(self->console, ConsoleLogSevere, "Cannot load connect-to-avr menu item.");
                } else {
                        g_signal_connect(G_OBJECT(mi_conn2dev), "activate",
                                         G_CALLBACK(__window_impl_on_connect2dev), self->pimpl);
                }
                GtkMenuItem* mi_helpabout = (GtkMenuItem*) gtk_builder_get_object(builder, "mi-helpabout");
                if (mi_helpabout == nullptr) {
                        console_log(self->console, ConsoleLogSevere, "Cannot load help about menu item.");
                } else {
                        g_signal_connect(G_OBJECT(mi_helpabout), "activate",
                                         G_CALLBACK(__window_impl_on_help_about), self->pimpl);
                }

                // Load buttons and connect signals.
                GtkButton* bt_capture = (GtkButton*) gtk_builder_get_object(builder, "bt-capture");
                if (bt_capture == nullptr) {
                        console_log(self->console, ConsoleLogSevere, "Cannot load device capture button.");
                } else {
                        g_signal_connect(G_OBJECT(bt_capture), "clicked",
                                         G_CALLBACK(__window_impl_capture_device), self->pimpl);
                }

                // Load impl UIs and connect signals.
                GtkDialog* dl_helpabout = (GtkDialog*) gtk_builder_get_object(builder, "dl-helpabout");
                GtkDialog* dl_conn = (GtkDialog*) gtk_builder_get_object(builder, "dl-connection");
                GtkComboBoxText* cb_selected = (GtkComboBoxText*) gtk_builder_get_object(builder, "cb-conn-selected");
                GtkButton* bt_conn_confirm = (GtkButton*) gtk_builder_get_object(builder, "bt-conn-confirm");
                GtkButton* bt_conn_cancel = (GtkButton*) gtk_builder_get_object(builder, "bt-conn-cancel");
                if (dl_helpabout == nullptr || dl_conn == nullptr || cb_selected == nullptr ||
                    bt_conn_confirm == nullptr || bt_conn_cancel == nullptr) {
                        console_log(self->console, ConsoleLogSevere, "Cannot load impl UIs");
                } else {
                        __window_impl_set_ui(self->pimpl,
                                             win_frame,
                                             sb_status,
                                             dl_helpabout,
                                             dl_conn,
                                             cb_selected,
                                             bt_conn_confirm,
                                             bt_conn_cancel);
                }
                g_timeout_add(100, __window_impl_fetch_device_console, self->pimpl);
                g_object_unref(builder);
        }

        window_update(self);
        gtk_window_set_position(GTK_WINDOW(win_frame), GTK_WIN_POS_CENTER);
        gtk_widget_realize(GTK_WIDGET(win_frame));

        __window_impl_push_status(self->pimpl, AERIAL_VERSION_STRING);

        // Install signals.
        g_signal_connect(GTK_WIDGET(win_frame), "destroy", gtk_main_quit, NULL);

        // Enter the main loop.
        gtk_widget_show_all(GTK_WIDGET(win_frame));
        gtk_main();

        __window_impl_pop_status(self->pimpl);
}
