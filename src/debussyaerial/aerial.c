#include <stdlib.h>
#include <gtk/gtk.h>

#define WINDOW_TITLE    "debussy - Aerial"
#define WINDOW_WIDTH    800
#define WINDOW_HEIGHT   600

int main(int argc, char* argv[])
{
        GtkWidget* win = NULL;

        // Initialize GTK+.
        g_log_set_handler("Gtk", G_LOG_LEVEL_WARNING, (GLogFunc) gtk_false, NULL);
        gtk_init(&argc, &argv);
        g_log_set_handler("Gtk", G_LOG_LEVEL_WARNING, g_log_default_handler, NULL);

        // Create the main window.
        win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        gtk_container_set_border_width(GTK_CONTAINER (win), 8);
        gtk_window_set_title(GTK_WINDOW(win), WINDOW_TITLE);
        gtk_window_set_position(GTK_WINDOW(win), GTK_WIN_POS_CENTER);
        gtk_widget_set_size_request(win, WINDOW_WIDTH, WINDOW_HEIGHT);
        gtk_widget_realize(win);
        g_signal_connect(win, "destroy", gtk_main_quit, NULL);

        // Enter the main loop.
        gtk_widget_show_all(win);
        gtk_main();
        return 0;
}
