#include <stdio.h>
#include <stdarg.h>
#include <aerial.h>
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


/*
 * <stdconsole> public
 */
void stdconsole_init(struct stdconsole* self)
{
        console_init(&self->__parent, (f_Console_Log) stdconsole_log, (f_Console_Free) stdconsole_free);
        stdconsole_log(self, "-------------- debussy stdconsole initialized ---------------");
}

void stdconsole_free(struct stdconsole* self)
{
        stdconsole_log(self, "-------------- debussy stdconsole freed ---------------");
}

int stdconsole_log(struct stdconsole* self, const char* format, ...)
{
        va_list list;
        va_start(list, format);
        int ret = vprintf(format, list);
        va_end(list);
        return ret;
}
