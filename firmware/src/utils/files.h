#pragma once

namespace utils::files {
    inline void iterateLines(
        fs::File& file,
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
                // ignore
            } else {
                if (lineLength <= lineBufferSize) {
                    buffer[lineLength] = c;
                    lineLength++;
                }
            }
        }
    }
    inline error_t iterateLines(
        const char* path,
        const std::function<void(const std::string_view&)>& callback,
        const size_t lineBufferSize = 1024
    ) {
        auto file = LittleFS.open(path, "r", false);
        if (!file) {
            return -1;
        }

        iterateLines(file, callback, lineBufferSize);

        file.close();

        return 0;
    }
}
