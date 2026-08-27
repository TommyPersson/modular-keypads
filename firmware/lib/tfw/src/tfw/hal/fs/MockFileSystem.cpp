#include "MockFileSystem.h"

namespace tfw::hal::fs {
    bool MockFileSystem::begin() {
        // Mock filesystem always initializes successfully
        return true;
    }

    std::shared_ptr<File> MockFileSystem::open(const char* path, FileMode mode, bool create) {
        if (!path) {
            return nullptr;
        }

        std::string pathStr(path);

        switch (mode) {
            case FileMode::Read: {
                // Try to open existing file for reading
                if (files.find(pathStr) != files.end()) {
                    return std::make_shared<MockFile>(*this, pathStr);
                }
                // File not found for reading
                return nullptr;
            }

            case FileMode::Write: {
                // Open for writing - truncates existing file or creates new one
                files[pathStr] = "";
                return std::make_shared<MockFile>(*this, pathStr);
            }

            case FileMode::Append: {
                // Open for appending
                if (files.find(pathStr) == files.end() && !create) {
                    return nullptr;
                }
                // Create if doesn't exist
                if (files.find(pathStr) == files.end()) {
                    files[pathStr] = "";
                }
                return std::make_shared<MockFile>(*this, pathStr);
            }

            default:
                return nullptr;
        }
    }

    bool MockFileSystem::remove(const char* path) {
        if (!path) {
            return false;
        }

        std::string pathStr(path);
        auto it = files.find(pathStr);
        if (it != files.end()) {
            files.erase(it);
            return true;
        }

        return false;
    }

    bool MockFileSystem::rename(const char* oldPath, const char* newPath) {
        if (!oldPath || !newPath) {
            return false;
        }

        std::string oldPathStr(oldPath);
        std::string newPathStr(newPath);

        auto it = files.find(oldPathStr);
        if (it != files.end()) {
            std::string content = it->second;
            files.erase(it);
            files[newPathStr] = content;
            return true;
        }

        return false;
    }

    bool MockFileSystem::exists(const char* path) {
        if (!path) {
            return false;
        }

        std::string pathStr(path);
        return files.find(pathStr) != files.end();
    }

    void MockFileSystem::clear() {
        files.clear();
    }

    std::string MockFileSystem::getFileContent(const char* path) const {
        if (!path) {
            return "";
        }

        std::string pathStr(path);
        auto it = files.find(pathStr);
        if (it != files.end()) {
            return it->second;
        }

        return "";
    }

    void MockFileSystem::setFileContent(const char* path, const std::string& content) {
        if (!path) {
            return;
        }

        std::string pathStr(path);
        files[pathStr] = content;
    }

    std::string& MockFileSystem::_getFileContent(const std::string& path) {
        return files[path];
    }

    const std::string& MockFileSystem::_getFileContent(const std::string& path) const {
        static std::string empty;
        auto it = files.find(path);
        if (it != files.end()) {
            return it->second;
        }
        return empty;
    }
}
