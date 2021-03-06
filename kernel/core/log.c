#include <stdarg.h>

#include <truth/format.h>
#include <truth/log.h>
#include <truth/panic.h>
#include <truth/types.h>

extern struct file *Log_File;


static enum log_level Log_Global_Level = Log_None;


char *Log_Prefix[Log_Count] = {
    "Debug: ",
    "Info:  ",
    "Warn:  ",
    "Error: ",
    "",
};


enum status log_init(enum log_level level, const char *name) {
    Log_Global_Level = level;
    return Log_File->init(name);
}


void log(enum log_level level, const char *message) {
    if (level >= Log_Global_Level) {
        fprintf(Log_File, "%s%s\n", Log_Prefix[level], message);
    }
}


void logf(enum log_level level, const char *message, ...) {
    if (level >= Log_Global_Level) {
        va_list args;
        va_start(args, message);
        fprintf(Log_File, "%s", Log_Prefix[level]);
        vfprintf(Log_File, message, args);
        va_end(args);
    }
}


void log_set_level(enum log_level level) {
    assert(level < Log_Count);
    Log_Global_Level = level;
}
