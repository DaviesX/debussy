#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <aerial.h>
#include <console.h>
#include <filesystem.h>
#include <connection.h>
#include <connmgr.h>

/*
 * <app> decl
 */
struct app {
        char*                   title;
        int                     w;
        int                     h;
        int*                    argc;
        char***                 argv;
        struct console*         console;

        GtkWindow*              win_frame;
        GtkStatusbar*           sb_status;
        GtkAboutDialog*         dl_helpabout;
        GtkDialog*              dl_connection;
        GtkComboBoxText*        cb_selected;
        GtkButton*              bt_conn_confirm;
        GtkButton*              bt_conn_cancel;
        GtkListBox*             lb_dev_files;

        struct conn_manager     conn_mgr;
        struct connection*      curr_conn;
};

/*
 * <app> public
 */
void app_init(struct app* self, int* argc, char*** argv);
void app_free(struct app* self);
void app_set_title(struct app* self, const char* title);
void app_set_size(struct app* self, const int w, const int h);
GtkBuilder* __app_builder_load(const struct app* self, const char* filename);
void app_run(struct app* self);


/*
 * <app> private
 */
GtkBuilder*     __app_builder_load(const struct app* self, const char* filename);
void*           __app_builder_get(const struct app* self, GtkBuilder* builder, const char* object_id);
void            __app_builder_free(const struct app* self, GtkBuilder* builder);
void            __app_push_status(struct app* self, const char* status);
void            __app_pop_status(struct app* self);
void            __app_setup_connection(struct app* self, struct connection* conn);
void            __app_update(struct app* self);
int             __app_show_message_box(const char* title, const char* message, const GtkMessageType type, GtkWindow* parent);
void            __app_on_quit(GtkWidget* widget, gpointer user_data);
void            __app_on_scan_connections(GtkMenuItem* menuitem, gpointer user_data);
void            __app_on_conn_confirm(GtkButton* button, gpointer user_data);
void            __app_on_connect2dev(GtkMenuItem* menuitem, gpointer user_data);
void            __app_on_add_local_device(GtkMenuItem* menuitem, gpointer user_data);
void            __app_on_help_about(GtkMenuItem* menuitem, gpointer user_data);
gboolean        __app_on_fetch_device_console(gpointer user_data);
void            __app_on_catch_device_files(GtkWidget* widget, gpointer user_data);
gboolean        __app_on_capture_device(GtkButton* button, gpointer user_data);


#define DEFAULT_WINDOW_TITLE    AERIAL_VERSION_STRING
#define DEFAULT_WINDOW_WIDTH    800
#define DEFAULT_WINDOW_HEIGHT   600

/*
 * <app> private
 */
/******************* Helpers *******************/
void __app_push_status(struct app* self, const char* status)
{
        if (self->sb_status) {
                guint id = gtk_statusbar_get_context_id(self->sb_status, AERIAL_VERSION_STRING);
                gtk_statusbar_push(self->sb_status, id, status);
        }
}

void __app_pop_status(struct app* self)
{
        if (self->sb_status) {
                guint id = gtk_statusbar_get_context_id(self->sb_status, AERIAL_VERSION_STRING);
                gtk_statusbar_pop(self->sb_status, id);
        }
}


void __app_update(struct app* self)
{
        if (self->win_frame) {
                gtk_window_set_title(GTK_WINDOW(self->win_frame), self->title);
                gtk_widget_set_size_request(GTK_WIDGET(self->win_frame), self->w, self->h);
        }
        __app_on_scan_connections(nullptr, self);
        __app_on_catch_device_files(nullptr, self);
}

void __app_setup_connection(struct app* self, struct connection* conn)
{
        if (conn) {
                char msg[256];
                const char* conn_str = conn_2string(conn);

                if (!conn_connect_to(conn)) {
                        sprintf(msg, "Connection %s cannot be established", (char*) conn_str), free((void*) conn_str);
                        __app_show_message_box(AERIAL_VERSION_STRING, msg,
                                               GTK_MESSAGE_ERROR, self->win_frame);
                } else {
                        // Connected to the device successfully.
                        if (self->curr_conn) {
                                // Disconnect the previous device.
                                __app_pop_status(self);
                                conn_disconnect(self->curr_conn);
                        }

                        self->curr_conn = conn;
                        __app_push_status(self, conn_str);
                        __app_update(self);

                        // Handle the widgets.
                        sprintf(msg, "Has been connected to %s", (char*) conn_str), free((void*) conn_str);
                        __app_show_message_box(AERIAL_VERSION_STRING, msg,
                                               GTK_MESSAGE_INFO, self->win_frame);
                        gtk_widget_hide(GTK_WIDGET(self->dl_connection));
                }
        } else {
                __app_show_message_box(AERIAL_VERSION_STRING, "Invalid connection",
                                       GTK_MESSAGE_ERROR, self->win_frame);
        }
}

int __app_show_message_box(const char* title, const char* message, const GtkMessageType type, GtkWindow* parent)
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
        gtk_dialog_run(GTK_DIALOG(dialog));
        if (type == GTK_MESSAGE_QUESTION) {
                gint ans = gtk_dialog_run(GTK_DIALOG (dialog));
                gtk_widget_destroy(dialog);
                return ans;
        } else {
                gtk_widget_destroy(dialog);
                return 0;
        }
}

const char* __app_show_file_chooser(const char* title, GtkFileChooserAction action, GtkWindow* parent)
{
        GtkWidget *dialog;
        gint res;

        dialog = gtk_file_chooser_dialog_new(title, parent, action,
                                             "_Cancel", GTK_RESPONSE_CANCEL,
                                             "_Open", GTK_RESPONSE_ACCEPT,
                                             nullptr);

        res = gtk_dialog_run(GTK_DIALOG(dialog));

        const char* filename;
        if (res == GTK_RESPONSE_ACCEPT)
                filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        else
                filename = nullptr;
        gtk_widget_destroy(dialog);

        return filename;
}
/******************* GUI utils *******************/

/******************* Callback controllers *******************/
void __app_on_quit(GtkWidget* widget, gpointer user_data)
{
        struct app* self = (struct app*) user_data;

        __app_pop_status(self);

        console_log(self->console, ConsoleLogNormal, "Switching back to stdconsole...");
        console_free(self->console);
        self->console = stdconsole_create();

        gtk_main_quit();
}


// Handling connections.
void __app_on_scan_connections(GtkMenuItem* menuitem, gpointer user_data)
{
        struct app* self = (struct app*) user_data;
        // Cache our current connection information,
        // then refresh all the AVR connections and retain the connect afterwards.
        char id[64] = {0};
        if (self->curr_conn)
                strcpy(id, self->curr_conn->id);
        connmgr_add_scanned_avr_connections(&self->conn_mgr, nullptr);
        self->curr_conn = connmgr_get_connection(&self->conn_mgr, id);

        console_log(self->console, ConsoleLogNormal, "List of devices >");
        const char** texts = connmgr_2strings(&self->conn_mgr);
        const struct connection** conns = connmgr_get_all_connections(&self->conn_mgr);
        int i;
        for (i = 0; i < connmgr_size(&self->conn_mgr); i ++) {
                console_log(self->console, ConsoleLogNormal, "Device ID > %s\n\t%s", conns[i]->id, texts[i]);
                free((void*) texts[i]);
        }
        free((void*) texts);
}

void __app_on_conn_confirm(GtkButton* button, gpointer user_data)
{
        struct app* self = (struct app*) user_data;
        if (button == self->bt_conn_confirm) {
                struct connection* conn = connmgr_get_connection(&self->conn_mgr,
                                              gtk_combo_box_get_active_id(GTK_COMBO_BOX(self->cb_selected)));
                __app_setup_connection(self, conn);
        } else {
                gtk_widget_hide(GTK_WIDGET(self->dl_connection));
        }
}

void __app_on_connect2dev(GtkMenuItem* menuitem, gpointer user_data)
{
        struct app* self = (struct app*) user_data;

        // Initialize selection box.
        const char** texts = connmgr_2strings(&self->conn_mgr);
        const struct connection** conns = connmgr_get_all_connections(&self->conn_mgr);
        gtk_combo_box_text_remove_all(self->cb_selected);
        int i;
        for (i = 0; i < connmgr_size(&self->conn_mgr); i ++) {
                gtk_combo_box_text_insert(self->cb_selected, -1, conns[i]->id, texts[i]);
        }

        // Run selection dialog.
        g_signal_connect(self->bt_conn_confirm, "clicked", G_CALLBACK(__app_on_conn_confirm), self);
        g_signal_connect(self->bt_conn_cancel, "clicked", G_CALLBACK(__app_on_conn_confirm), self);
        g_signal_connect_swapped(self->dl_connection, "response", G_CALLBACK(gtk_widget_hide), self->dl_connection);
        gtk_dialog_run(self->dl_connection);
        g_signal_handlers_disconnect_by_func(self->bt_conn_confirm, G_CALLBACK(__app_on_conn_confirm), self);
        g_signal_handlers_disconnect_by_func(self->bt_conn_cancel, G_CALLBACK(__app_on_conn_confirm), self);

        // Clean up texts.
        for (i = 0; i < connmgr_size(&self->conn_mgr); i ++) {
                free((void*) texts[i]);
        }
        free((void*) texts);
}

void __app_on_add_local_device(GtkMenuItem* menuitem, gpointer user_data)
{
        struct app* self = (struct app*) user_data;

        const char* filename = __app_show_file_chooser("Add local device",
                                                       GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
                                                       self->win_frame);
        if (filename) {
                console_log(self->console, ConsoleLogNormal, "Connecting to local device %s", filename);
                struct filesystem* fs = (struct filesystem*) fs_posix_create(filename);
                if (fs)
                        connmgr_add_local_connection(&self->conn_mgr, fs, self->console);
                else
                        console_log(self->console, ConsoleLogSevere,
                                   "Failed to form a posix file system on the device %s", filename);
                g_free((void*) filename);
        }
}

// Help about.
void __app_on_help_about(GtkMenuItem* menuitem, gpointer user_data)
{
        struct app* self = (struct app*) user_data;
        g_signal_connect_swapped(self->dl_helpabout, "response", G_CALLBACK(gtk_widget_hide), self->dl_helpabout);
        gtk_dialog_run(GTK_DIALOG(self->dl_helpabout));
}

// Device access.
gboolean __app_on_fetch_device_console(gpointer user_data)
{
        struct app* self = (struct app*) user_data;
        const char* msg = self->curr_conn != nullptr ? conn_gets(self->curr_conn) : nullptr;
        if (msg != nullptr)
                console_log(self->console, ConsoleLogNormal, "device: %s", (char*) msg), free((void*) msg);
        return TRUE;
}

static void __remove_all_list_items(gpointer data, gpointer user_data)
{
        struct app* self = (struct app*) user_data;

        gtk_container_remove(GTK_CONTAINER(self->lb_dev_files), data);
}

void __app_on_catch_device_files(GtkWidget* widget, gpointer user_data)
{
        struct app* self = (struct app*) user_data;

        // Empty the list box.
        GList* all_widgets = gtk_container_get_children(GTK_CONTAINER(self->lb_dev_files));
        g_list_foreach(all_widgets, __remove_all_list_items, self);

        if (self->curr_conn) {
                // Get file entries into the list box.
        } else {
                GtkWidget* lb_no_dev = gtk_label_new("No device is connected");
                gtk_container_add(GTK_CONTAINER(self->lb_dev_files), lb_no_dev);
        }
        gtk_widget_show_all(GTK_WIDGET(self->lb_dev_files));
}

gboolean __app_on_capture_device(GtkButton* button, gpointer user_data)
{
        // struct app* self = (struct app*) user_data;
        return TRUE;
}
/******************* Callback controllers *******************/

/*
 * <app> public
 */
void app_init(struct app* self, int* argc, char*** argv)
{
        memset(self, 0, sizeof(*self));
        self->argc = argc;
        self->argv = argv;
        self->console = stdconsole_create();

        connmgr_init(&self->conn_mgr);

        app_set_size(self, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
        app_set_title(self, DEFAULT_WINDOW_TITLE);
}

void app_free(struct app* self)
{
        console_free(self->console);
        connmgr_free(&self->conn_mgr);

        free(self->console);
}

void app_set_title(struct app* self, const char* title)
{
        free(self->title);
        self->title = strdup(title);
}

void app_set_size(struct app* self, const int w, const int h)
{
        self->w = w;
        self->h = h;
}

GtkBuilder* __app_builder_load(const struct app* self, const char* filename)
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

void* __app_builder_get(const struct app* self, GtkBuilder* builder, const char* object_id)
{
        void* widget = (void*) gtk_builder_get_object(builder, object_id);
        if (widget == nullptr) {
                console_log(self->console, ConsoleLogSevere, "Failed to load widget %s", object_id);
        }
        return widget;
}

void __app_builder_free(const struct app* self, GtkBuilder* builder)
{
        g_object_unref(builder);
}

void app_run(struct app* self)
{
        console_log(self->console, ConsoleLogNormal, "Initializing GTK+...");
        g_log_set_handler("Gtk", G_LOG_LEVEL_WARNING, (GLogFunc) gtk_false, NULL);
        gtk_init(self->argc, self->argv);
        g_log_set_handler("Gtk", G_LOG_LEVEL_WARNING, g_log_default_handler, NULL);

        // Create the main app from builder.
        GtkBuilder* builder = __app_builder_load(self, "data/debussy-aerial.glade");
        if (builder == nullptr) {
                console_log(self->console, ConsoleLogSevere, "Failed to build aerial UI. Use fallback app.");
                self->win_frame = (GtkWindow*) gtk_window_new(GTK_WINDOW_TOPLEVEL);
        } else {
                console_log(self->console, ConsoleLogSevere, "Window has been loaded.");
                self->win_frame = __app_builder_get(self, builder, "win-main-frame");
                self->sb_status = (GtkStatusbar*) __app_builder_get(self, builder, "sb-main");
                GtkTextView* tv_console = (GtkTextView*) __app_builder_get(self, builder, "tv-console");
                if (tv_console == nullptr) {
                        console_log(self->console, ConsoleLogSevere, "Cannot load gtk console widget. Use fallback console.");
                } else {
                        console_log(self->console, ConsoleLogNormal, "Connecting to gtk console...");
                        console_free(self->console);
                        self->console = gtkconsole_create(tv_console);
                }

                // Load menus and connect signals.
                GtkMenuItem* mi_scan_conn = __app_builder_get(self, builder, "mi-scan-connection");
                if (mi_scan_conn)
                        g_signal_connect(G_OBJECT(mi_scan_conn), "activate",
                                         G_CALLBACK(__app_on_scan_connections), self);

                GtkMenuItem* mi_conn2dev = __app_builder_get(self, builder, "mi-connect2device");
                if (mi_conn2dev)
                        g_signal_connect(G_OBJECT(mi_conn2dev), "activate",
                                         G_CALLBACK(__app_on_connect2dev), self);

                GtkMenuItem* mi_conn2localdev = __app_builder_get(self, builder, "mi-add-local-dev");
                if (mi_conn2localdev)
                        g_signal_connect(G_OBJECT(mi_conn2localdev), "activate",
                                         G_CALLBACK(__app_on_add_local_device), self);

                GtkMenuItem* mi_helpabout = __app_builder_get(self, builder, "mi-helpabout");
                if (mi_helpabout)
                        g_signal_connect(G_OBJECT(mi_helpabout), "activate",
                                         G_CALLBACK(__app_on_help_about), self);

                // Load buttons and connect signals.
                GtkButton* bt_capture = __app_builder_get(self, builder, "bt-capture");
                if (bt_capture)
                        g_signal_connect(G_OBJECT(bt_capture), "clicked",
                                         G_CALLBACK(__app_on_capture_device), self);

                // Load impl UIs and connect signals.
                self->dl_helpabout = __app_builder_get(self, builder, "dl-helpabout");
                self->dl_connection = __app_builder_get(self, builder, "dl-connection");
                self->cb_selected = __app_builder_get(self, builder, "cb-conn-selected");
                self->bt_conn_confirm = __app_builder_get(self, builder, "bt-conn-confirm");
                self->bt_conn_cancel = __app_builder_get(self, builder, "bt-conn-cancel");
                self->lb_dev_files = __app_builder_get(self, builder, "lb-dev-files");

                g_timeout_add(100, __app_on_fetch_device_console, self);
                __app_builder_free(self, builder);
        }

        __app_update(self);
        gtk_window_set_position(GTK_WINDOW(self->win_frame), GTK_WIN_POS_CENTER);
        gtk_widget_realize(GTK_WIDGET(self->win_frame));

        __app_push_status(self, AERIAL_VERSION_STRING);

        // Install signals.
        g_signal_connect(GTK_WIDGET(self->win_frame), "destroy", G_CALLBACK(__app_on_quit), self);

        // Enter the main loop.
        gtk_widget_show_all(GTK_WIDGET(self->win_frame));
        gtk_main();
}


int main(int argc, char* argv[])
{
        struct app win;
        app_init(&win, &argc, &argv), app_run(&win), app_free(&win);
        return 0;
}
