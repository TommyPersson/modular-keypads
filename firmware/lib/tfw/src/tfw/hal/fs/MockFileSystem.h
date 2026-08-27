#pragma once

#include "FileSystem.h"
#include "MockFile.h"
#include <map>
#include <string>
#include <memory>

namespace tfw::hal::fs {
    class MockFileSystem : public FileSystem {
    public:
        MockFileSystem() = default;

        bool begin() override;
        std::shared_ptr<File> open(const char* path, FileMode mode, bool create = false) override;
        bool remove(const char* path) override;
        bool rename(const char* oldPath, const char* newPath) override;
        bool exists(const char* path) override;
        void clear();
        std::string getFileContent(const char* path) const;
        void setFileContent(const char* path, const std::string& content);
        std::string& _getFileContent(const std::string& path);
        const std::string& _getFileContent(const std::string& path) const;

    private:
        std::map<std::string, std::string> files;
        friend class MockFile;
    };
}
