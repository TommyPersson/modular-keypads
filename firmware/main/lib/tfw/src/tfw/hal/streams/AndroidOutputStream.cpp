#ifdef ARDUINO

#include "OutputStream.h"

#include <Print.h>

namespace tfw::hal::streams {
    /// Arduino-specific implementation of OutputStream that wraps Print
    class AndroidOutputStream : public OutputStream {
    public:
        explicit AndroidOutputStream(Print& printObject) : printObject(printObject) {
        }

        size_t write(uint8_t c) override {
            return printObject.write(c);
        }

        size_t print(const char* str) override {
            return printObject.print(str);
        }

        size_t println(const char* str) override {
            return printObject.println(str);
        }

        int vprintf(const char* format, va_list args) override {
            return printObject.vprintf(format, args);
        }

        int printf(const char* format, ...) override {
            va_list args;
            va_start(args, format);
            int result = vprintf(format, args);
            va_end(args);
            return result;
        }

        void flush() override {
            printObject.flush();
        }

    private:
        Print& printObject;
    };

    /// Factory function to create an Arduino-specific OutputStream
    OutputStream* createArduinoOutputStream(Print& printObject) {
        return new AndroidOutputStream(printObject);
    }
}

#endif // ARDUINO
