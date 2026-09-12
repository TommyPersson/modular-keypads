#pragma once

#include <cstdarg>
#include <cstddef>
#include <cstdint>

#ifdef ARDUINO
#include <Print.h>
#endif

namespace tfw::hal::streams {
    /// Abstract interface for output streams (decoupled from Arduino's Print class)
    class OutputStream {
    public:
        virtual ~OutputStream() = default;

        /// Write a single character (returns number of bytes written)
        virtual size_t write(uint8_t c) = 0;

        /// Write a C-string (returns number of bytes written)
        virtual size_t print(const char* str) = 0;

        /// Write a C-string followed by newline (returns number of bytes written)
        virtual size_t println(const char* str) = 0;

        /// Write formatted output (printf-style, returns number of bytes written)
        virtual int vprintf(const char* format, va_list args) = 0;

        /// Write formatted output (printf-style, variadic, returns number of bytes written)
        virtual int printf(const char* format, ...) = 0;

        /// Flush any buffered output
        virtual void flush() = 0;
    };

#ifdef ARDUINO

    /// Factory function to create an Arduino-specific OutputStream wrapping a Print reference
    /// Only available when building for Arduino
    OutputStream* createArduinoOutputStream(Print& printObject);

#endif // ARDUINO
}
