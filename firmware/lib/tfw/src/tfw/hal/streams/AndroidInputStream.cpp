#ifdef ARDUINO

#include "InputStream.h"

#include <Stream.h>

namespace tfw::hal::streams {
    /// Arduino-specific implementation of InputStream that wraps Stream
    class AndroidInputStream : public InputStream {
    public:
        explicit AndroidInputStream(Stream& streamObject) : streamObject(streamObject) {
        }

        int available() override {
            return streamObject.available();
        }

        int read() override {
            return streamObject.read();
        }

        int peek() override {
            return streamObject.peek();
        }

    private:
        Stream& streamObject;
    };

    /// Factory function to create an Arduino-specific InputStream
    InputStream* createArduinoInputStream(Stream& streamObject) {
        return new AndroidInputStream(streamObject);
    }
}

#endif // ARDUINO
