#ifndef CONSOLE_H_INCLUDED
#define CONSOLE_H_INCLUDED


/*
 * <console> decl
 */
struct console;
enum ConsoleLogLevel {
        ConsoleLogNormal,
        ConsoleLogMild,
        ConsoleLogSevere,
        ConsoleLogCritical
};
typedef int (*f_Console_Log) (struct console* self, const int log_level, const char* format, ...);
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

#define console_log(__self, __log_level, __format, ...)         \
        ((__self) == nullptr ? -1 : (__self)->f_console_log(__self, (__log_level), __format, ##__VA_ARGS__))

/*
 * <stdconsole> decl
 */
struct stdconsole {
        struct console __parent;
};

/*
 * <stdconsole> public
 */
struct console* stdconsole_create();
void stdconsole_init(struct stdconsole* self);
void stdconsole_free(struct stdconsole* self);
int stdconsole_log(const struct stdconsole* self, const int log_level, const char* format, ...);

/*
 * <gtkconsole> decl
 */
typedef struct _GtkTextView GtkTextView;
typedef struct _GtkTextBuffer GtkTextBuffer;
struct gtkconsole {
        struct console          __parent;
        struct stdconsole*      stdconsole;
        GtkTextView*            text_view;
        GtkTextBuffer*          text_buffer;
};

/*
 * <gtkconsole> public
 */
struct console* gtkconsole_create(GtkTextView* target_widget);
void gtkconsole_init(struct gtkconsole* self, GtkTextView* target_widget);
void gtkconsole_free(struct gtkconsole* self);
int gtkconsole_log(const struct gtkconsole* self, const int log_level, const char* format, ...);


#endif // CONSOLE_H_INCLUDED
