#pragma once

#include <cstdint>

#ifdef ARDUINO
#include <Stream.h>
#endif

namespace tfw::hal::streams {
    /// Abstract interface for input streams (decoupled from Arduino's Stream class)
    class InputStream {
    public:
        virtual ~InputStream() = default;

        /// Check if data is available to read
        virtual int available() = 0;

        /// Read one byte of data (-1 if none available)
        virtual int read() = 0;

        /// Peek at the next byte without consuming it
        virtual int peek() = 0;
    };

#ifdef ARDUINO

    /// Factory function to create an Arduino-specific InputStream wrapping a Stream reference
    /// Only available when building for Arduino
    InputStream* createArduinoInputStream(Stream& streamObject);

#endif // ARDUINO
}
