#ifndef CONSOLE_H_INCLUDED
#define CONSOLE_H_INCLUDED


/*
 * <console> decl
 */
struct console;
typedef int (*f_Console_Log) (struct console* self, const char* format, ...);
typedef void (*f_Console_Free) (struct console* self);
struct console {
        f_Console_Log   f_console_log;
        f_Console_Free  f_free;
};

/*
 * <console> public
 */
void console_init(struct console* self, f_Console_Log f_console_log, f_Console_Free f_free);
void console_free(struct console* self);

#define console_log(__self, __format, ...)                              \
        ((__self)->f_console_log(__self, __format, ##__VA_ARGS__))

/*
 * <stdconsole> decl
 */
struct stdconsole {
        struct console __parent;
};
void stdconsole_init(struct stdconsole* self);
void stdconsole_free(struct stdconsole* self);
int stdconsole_log(struct stdconsole* self, const char* format, ...);


#endif // CONSOLE_H_INCLUDED
