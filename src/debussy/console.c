#include <avr.h>
#include <console.h>


/*
 * <console> public
 */
void console_init(struct console* self, f_Console_Log f_console_log, f_Console_Free f_free)
{
        self->f_console_log = f_console_log;
        self->f_free = f_free;
}

void console_free(struct console* self)
{
        self->f_free(self);
}

#ifdef ARCH_X86_64
#  include <gtk/gtk.h>
#  include <stdio.h>
#  include <stdlib.h>
#  include <stdarg.h>
#  include <string.h>
#  include <assert.h>

/*
 * <stdconsole> public
 */
struct console* stdconsole_create()
{
        struct stdconsole* self = malloc(sizeof(*self));
        stdconsole_init(self);
        return &self->__parent;
}

void stdconsole_init(struct stdconsole* self)
{
        console_init(&self->__parent, (f_Console_Log) stdconsole_log, (f_Console_Free) stdconsole_free);
        stdconsole_log(self, ConsoleLogNormal,
                       "-------------- %s - stdconsole connected ---------------", DEBUSSY_VERSION_STRING);
}

void stdconsole_free(struct stdconsole* self)
{
        stdconsole_log(self, ConsoleLogNormal,
                       "-------------- %s - stdconsole disconnected ---------------", DEBUSSY_VERSION_STRING);
}

int stdconsole_log(const struct stdconsole* self, const int log_level, const char* format, ...)
{
        va_list list;
        va_start(list, format);

        int ret = vprintf(format, list);
        puts("");

        va_end(list);
        return ret;
}


/*
 * <gtkconsole> public
 */
struct console* gtkconsole_create(GtkTextView* target_widget)
{
        struct gtkconsole* self = malloc(sizeof(*self));
        gtkconsole_init(self, target_widget);
        return &self->__parent;
}

void gtkconsole_init(struct gtkconsole* self, GtkTextView* target_widget)
{
        console_init(&self->__parent, (f_Console_Log) gtkconsole_log, (f_Console_Free) gtkconsole_free);

        self->text_view = target_widget;
        self->text_buffer = gtk_text_view_get_buffer(self->text_view);
        assert(self->text_buffer != nullptr);

        gtk_text_view_set_editable(self->text_view, true);
        self->stdconsole = (struct stdconsole*) stdconsole_create();

        gtkconsole_log(self, ConsoleLogNormal,
                       "-------------- %s - gtkconsole connected ---------------", DEBUSSY_VERSION_STRING);
}

void gtkconsole_free(struct gtkconsole* self)
{
        gtkconsole_log(self, ConsoleLogNormal,
                       "-------------- %s - gtkconsole disconnected ---------------", DEBUSSY_VERSION_STRING);
        stdconsole_free(self->stdconsole);
}

int gtkconsole_log(const struct gtkconsole* self, const int log_level, const char* format, ...)
{
        va_list list;
        va_start(list, format);

        char* buf = nullptr;
        int ret = vasprintf(&buf, format, list);
        if (ret == -1) {
                gtkconsole_log(self, ConsoleLogMild, "Malformed format: %s", format);
                return ret;
        }
        GtkTextIter iter;
        gtk_text_buffer_get_end_iter(self->text_buffer, &iter);
        gtk_text_buffer_insert(self->text_buffer, &iter, buf, strlen(buf));
        gtk_text_buffer_insert(self->text_buffer, &iter, "\n", strlen("\n"));
        gtk_text_view_scroll_to_iter(self->text_view, &iter, 0, true, 0, 0);

        stdconsole_log(self->stdconsole, log_level, "%s", buf);

        va_end(list);
        return ret;
}

#endif // ARCH_X86_64
