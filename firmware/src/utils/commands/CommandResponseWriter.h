#pragma once

#include <cstdarg>
#include <string>

#include <Print.h>

namespace utils::commands {
    class CommandResponseWriter {
    public:
        CommandResponseWriter(int commandId, Print& outputStream);
        ~CommandResponseWriter();

        void writeLine(const std::string_view& line) {
            writeLineF("%.*s", line.length(), line.data());
        }

        void writeLineF(const char* format, ...) __attribute__((format(printf, 2, 3))) {
            va_list args;
            va_start(args, format);

            outputStream.printf("%%%i.%i:", commandId, sequenceNumber);
            outputStream.vprintf(format, args);
            outputStream.print("\n");

            va_end(args);

            sequenceNumber += 1;
        }

    private:
        const int commandId;
        int sequenceNumber;
        Print& outputStream;
    };
}