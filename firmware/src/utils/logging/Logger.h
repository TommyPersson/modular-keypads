#pragma once

#include <cstdarg>
#include <memory>
#include <Print.h>
#include <optional>

namespace utils::logging {
    class Logger {
    public:
        explicit Logger(std::optional<Print*>& outputStream, const std::string& name);

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
            const auto ostream = outputStream.value_or(nullptr);
            if (ostream == nullptr) {
                return;
            }

            ostream->print("#");
            ostream->print(level);
            ostream->print(":");
            if (!name.empty()) {
                ostream->print(name.c_str());
                ostream->print(":");
            }
            ostream->vprintf(format, args);
            ostream->print("\n");
            ostream->flush();
        };

        std::optional<Print*>& outputStream;
        std::string name;
    };

    void initialize(Print* outputStream);
    std::shared_ptr<Logger> createLogger(const std::string& name);
}
