#ifdef ARDUINO

#include "ArduinoFile.h"

namespace tfw::hal::fs {
    ArduinoFile::ArduinoFile(::fs::File file) : file(file) {}

    ArduinoFile::~ArduinoFile() {
        if (file) {
            file.close();
        }
    }

    int ArduinoFile::available() {
        return file.available();
    }

    int ArduinoFile::read() {
        return file.read();
    }

    size_t ArduinoFile::write(uint8_t c) {
        return file.write(c);
    }

    size_t ArduinoFile::print(const char* str) {
        return file.print(str);
    }

    size_t ArduinoFile::println(const char* str) {
        return file.println(str);
    }

    void ArduinoFile::close() {
        file.close();
    }
}

#endif // ARDUINO
