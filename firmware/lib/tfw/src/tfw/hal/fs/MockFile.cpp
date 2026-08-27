#include "MockFile.h"
#include "MockFileSystem.h"

namespace tfw::hal::fs {
    MockFile::MockFile(MockFileSystem& fileSystem, const std::string& filePath)
        : fileSystem(fileSystem), filePath(filePath), readPosition(0), isOpen(true) {}

    std::string& MockFile::getFileContent() {
        return fileSystem._getFileContent(filePath);
    }

    const std::string& MockFile::getFileContent() const {
        return fileSystem._getFileContent(filePath);
    }

    int MockFile::available() {
        if (!isOpen) return 0;
        const auto& content = getFileContent();
        return static_cast<int>(content.size() - readPosition);
    }

    int MockFile::read() {
        if (!isOpen) return -1;
        const auto& content = getFileContent();
        if (readPosition >= content.size()) {
            return -1;
        }
        return static_cast<unsigned char>(content[readPosition++]);
    }

    size_t MockFile::write(uint8_t c) {
        if (!isOpen) return 0;
        auto& content = getFileContent();
        content.push_back(static_cast<char>(c));
        return 1;
    }

    size_t MockFile::print(const char* str) {
        if (!isOpen || !str) return 0;
        auto& content = getFileContent();
        size_t written = 0;
        while (*str) {
            content.push_back(*str);
            ++str;
            ++written;
        }
        return written;
    }

    size_t MockFile::println(const char* str) {
        size_t written = print(str);
        if (isOpen) {
            auto& content = getFileContent();
            content.push_back('\n');
            ++written;
        }
        return written;
    }

    void MockFile::close() {
        isOpen = false;
        readPosition = 0;
    }
}
