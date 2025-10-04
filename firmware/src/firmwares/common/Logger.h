#pragma once
#include <Stream.h>
#include <cstdarg>

// TODO stack strings

class Logger {
public:
    explicit Logger(Stream& outputStream);

    void info(const char* format, ...) __attribute__((format(printf, 2, 3))) {
        String finalFormat;
        finalFormat.concat("#info:");
        finalFormat.concat(format);
        finalFormat.concat("\n");

        va_list va;
        va_start(va, format);
        outputStream.vprintf(finalFormat.c_str(), va);
        va_end(va);
    };

    void debug(const char* format, ...) __attribute__((format(printf, 2, 3))) {
        String finalFormat;
        finalFormat.concat("#debug:");
        finalFormat.concat(format);
        finalFormat.concat("\n");

        va_list va;
        va_start(va, format);
        outputStream.vprintf(finalFormat.c_str(), va);
        va_end(va);
    };

    void warn(const char* format, ...) __attribute__((format(printf, 2, 3))) {
        String finalFormat;
        finalFormat.concat("#warn:");
        finalFormat.concat(format);
        finalFormat.concat("\n");

        va_list va;
        va_start(va, format);
        outputStream.vprintf(finalFormat.c_str(), va);
        va_end(va);
    };

    void error(const char* format, ...) __attribute__((format(printf, 2, 3))) {
        String finalFormat;
        finalFormat.concat("#error:");
        finalFormat.concat(format);
        finalFormat.concat("\n");

        va_list va;
        va_start(va, format);
        outputStream.vprintf(finalFormat.c_str(), va);
        va_end(va);
    };

private:

    Stream& outputStream;
};
