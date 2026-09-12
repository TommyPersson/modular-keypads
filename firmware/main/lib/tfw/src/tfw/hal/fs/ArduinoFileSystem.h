#pragma once

#include "FileSystem.h"

#ifdef ARDUINO

namespace tfw::hal::fs {
    class ArduinoFileSystem : public FileSystem {
    public:
        bool begin() override;
        std::shared_ptr<File> open(const char* path, FileMode mode, bool create) override;
        bool remove(const char* path) override;
        bool rename(const char* oldPath, const char* newPath) override;
        bool exists(const char* path) override;
    };
}

#endif // ARDUINO
