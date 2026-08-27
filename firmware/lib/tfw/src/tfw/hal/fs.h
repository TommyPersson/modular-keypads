#pragma once

#include <tfw/hal/fs/File.h>
#include <tfw/hal/fs/FileSystem.h>
#include <tfw/hal/fs/FileUtilities.h>
#include <memory>

#ifdef ARDUINO
#include <tfw/hal/fs/ArduinoFile.h>
#include <tfw/hal/fs/ArduinoFileSystem.h>

namespace tfw::hal::fs {
    /// Factory function to create a filesystem implementation for Arduino
    /// Only available when building for Arduino
    /// Returns a shared_ptr to ArduinoFileSystem wrapping LittleFS
    inline std::shared_ptr<FileSystem> createArduinoFileSystem() {
        return std::make_shared<ArduinoFileSystem>();
    }
}

#endif // ARDUINO
