#pragma once

#include "File.h"

#ifdef ARDUINO
#include <FS.h>

namespace tfw::hal::fs {
    class ArduinoFile : public File {
    public:
        explicit ArduinoFile(::fs::File file);
        ~ArduinoFile() override;

        int available() override;
        int read() override;
        size_t write(uint8_t c) override;
        size_t print(const char* str) override;
        size_t println(const char* str) override;
        void close() override;

    private:
        ::fs::File file;
    };
}

#endif // ARDUINO
