#include <cstdio>
#include <string>
#include <cstring>
#include <stdarg.h>

#include "Logger.h"

void log_printf(LogLevel level, const char* format, ...)
{
    if(global_verbosity > level) return;

    va_list vargs;
    va_start(vargs, format);

    FILE* stream = stdout;
    if(level == ERROR || level == WARN) stream = stderr;

    fprintf(stream, "[%s] ", level_prefix(level));
    vfprintf(stream, format, vargs);
    fprintf(stream, "\n");
}

const char* level_prefix(LogLevel level)
{
    if(level == DEBUG) return "DEBUG";
    if(level == INFO) return "INFO" ;
    if(level == WARN) return "WARN";
    if(level == ERROR) return "ERROR";

    return "DEBUG";
}