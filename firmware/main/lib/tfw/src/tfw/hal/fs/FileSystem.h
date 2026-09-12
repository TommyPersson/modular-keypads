#pragma once

#include "File.h"
#include <memory>

namespace tfw::hal::fs {
    enum class FileMode {
        Read = 0,
        Write = 1,
        Append = 2,
    };

    class FileSystem {
    public:
        virtual ~FileSystem() = default;

        virtual bool begin() = 0;
        virtual std::shared_ptr<File> open(const char* path, FileMode mode, bool create) = 0;
        virtual bool remove(const char* path) = 0;
        virtual bool rename(const char* oldPath, const char* newPath) = 0;
        virtual bool exists(const char* path) = 0;
    };
}
