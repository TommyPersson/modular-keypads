#pragma once

#include "File.h"
#include <string>
#include <memory>

namespace tfw::hal::fs {
    class MockFileSystem;  // Forward declaration

    class MockFile : public File {
    public:
        MockFile(MockFileSystem& fileSystem, const std::string& filePath);
        ~MockFile() override = default;

        int available() override;
        int read() override;
        size_t write(uint8_t c) override;
        size_t print(const char* str) override;
        size_t println(const char* str) override;
        void close() override;

    private:
        MockFileSystem& fileSystem;
        std::string filePath;
        size_t readPosition = 0;
        bool isOpen = true;

        std::string& getFileContent();
        const std::string& getFileContent() const;
    };
}
