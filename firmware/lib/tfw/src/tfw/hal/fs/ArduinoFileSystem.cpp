#ifdef ARDUINO

#include "ArduinoFileSystem.h"
#include "ArduinoFile.h"

#include <LittleFS.h>

namespace tfw::hal::fs {
    bool ArduinoFileSystem::begin() {
        return LittleFS.begin(false);
    }

    std::shared_ptr<File> ArduinoFileSystem::open(const char* path, FileMode mode, bool create) {
        const char* modeStr;

        switch (mode) {
            case FileMode::Read:
                modeStr = "r";
                break;
            case FileMode::Write:
                modeStr = "w";
                break;
            case FileMode::Append:
                modeStr = "a";
                break;
            default:
                return nullptr;
        }

        ::fs::File file = LittleFS.open(path, modeStr, create);
        if (!file) {
            return nullptr;
        }

        return std::make_shared<ArduinoFile>(file);
    }

    bool ArduinoFileSystem::remove(const char* path) {
        return LittleFS.remove(path);
    }

    bool ArduinoFileSystem::rename(const char* oldPath, const char* newPath) {
        return LittleFS.rename(oldPath, newPath);
    }

    bool ArduinoFileSystem::exists(const char* path) {
        return LittleFS.exists(path);
    }
}

#endif // ARDUINO
