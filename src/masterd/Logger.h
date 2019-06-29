#ifndef MASTEROPTIONS_LOGGER_H
#define MASTEROPTIONS_LOGGER_H

enum LogLevel
{
   DEBUG,
   INFO,
   WARN,
   ERROR
};

extern LogLevel global_verbosity;

void log_printf(LogLevel level, const char* format, ...);

const char* level_prefix(LogLevel level);

#endif //MASTEROPTIONS_LOGGER_H
