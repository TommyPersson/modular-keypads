#pragma once

#include <cstdint>
#include <cstddef>

namespace tfw::hal::fs {
    class File {
    public:
        virtual ~File() = default;
        virtual int available() = 0;
        virtual int read() = 0;
        virtual size_t write(uint8_t c) = 0;
        virtual size_t print(const char* str) = 0;
        virtual size_t println(const char* str) = 0;
        virtual void close() = 0;
    };
}
