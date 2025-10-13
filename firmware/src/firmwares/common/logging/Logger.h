#pragma once

#include <Stream.h>
#include <cstdarg>

class Logger {
public:
    explicit Logger(Stream& outputStream);

    void info(const char* format, ...) const __attribute__((format(printf, 2, 3))) {
        va_list args;
        va_start(args, format);
        vlog("info", format, args);
        va_end(args);
    };

    void debug(const char* format, ...) const __attribute__((format(printf, 2, 3))) {
        va_list args;
        va_start(args, format);
        vlog("debug", format, args);
        va_end(args);
    };

    void warn(const char* format, ...) const __attribute__((format(printf, 2, 3))) {
        va_list args;
        va_start(args, format);
        vlog("warn", format, args);
        va_end(args);
    };

    void error(const char* format, ...) const __attribute__((format(printf, 2, 3))) {
        va_list args;
        va_start(args, format);
        vlog("error", format, args);
        va_end(args);
    };

private:
    void vlog(const char* level, const char* format, const va_list args) const {
        outputStream.print("#");
        outputStream.print(level);
        outputStream.print(":");
        outputStream.vprintf(format, args);
        outputStream.print("\n");
        outputStream.flush();
    };

    Stream& outputStream;
};
