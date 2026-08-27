#pragma once

#include "FileSystem.h"
#include <functional>
#include <memory>

namespace tfw::hal::fs {
    inline void iterateLines(
        File& file,
        const std::function<void(const std::string_view&)>& callback,
        const size_t lineBufferSize = 1024
    ) {
        char buffer[lineBufferSize];
        size_t lineLength = 0;

        while (file.available()) {
            char c = file.read();

            if (c == '\n') {
                std::string_view line{buffer, lineLength};

                if (line.length() > 0) {
                    callback(line);
                }

                lineLength = 0;
            } else if (c == '\r') {
                // ignore carriage return
            } else {
                if (lineLength < lineBufferSize) {
                    buffer[lineLength] = c;
                    lineLength++;
                }
            }
        }
    }

    inline int iterateLines(
        FileSystem& filesystem,
        const char* path,
        const std::function<void(const std::string_view&)>& callback,
        const size_t lineBufferSize = 1024
    ) {
        const auto file = filesystem.open(path, FileMode::Read, false);
        if (!file) {
            return -1;
        }

        iterateLines(*file, callback, lineBufferSize);

        file->close();

        return 0;
    }
}
