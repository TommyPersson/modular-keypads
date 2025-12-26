#include "MacroStorage.h"

#include <Preferences.h>
#include <FS.h>
#include <SPIFFS.h>
#include <LittleFS.h>

#include "firmwares/common/logging/Logger.h"


namespace {
    auto logger = common::logging::createLogger("MacroStorage");
    bool beginResult = false;
    bool hasBegun = false;
    bool hasRead = false;
    Preferences prefs;
    auto filePath = "/macro-definitions.txt";
}

namespace {
    void listDir(const char* path) {
        auto root = LittleFS.open(path);
        if (!root) {
            logger->error("Failed to open '%s' for reading!", path);
            return;
        }

        logger->info("dir = %s", path);

        auto nextFile = root.openNextFile();
        while (nextFile) {
            if (nextFile.isDirectory()) {
                listDir(nextFile.path());
            } else {
                logger->info("file = %s", nextFile.name());
            }

            nextFile = root.openNextFile();
        }
    }

    void checkForPath(const char* path) {
        if (!LittleFS.exists(path)) {
            logger->info("Path '%s' does NOT exist!", path);
        } else {
            logger->info("Path '%s' DOES exist!", path);
        }
    }
}

// TODO do we need to be careful with allocations here?
void common::macros::MacroStorage::setup() {}

error_t common::macros::MacroStorage::write(const Macro& macro) {
    if (!hasBegun) {
        beginResult = LittleFS.begin(false);
        if (LittleFS.exists(filePath)) {
            LittleFS.remove(filePath);
        }
        hasBegun = true;
    }

    logger->debug("Would try to save macro '%s'", macro.name.c_str());
/* TODO re-implement
    auto file = LittleFS.open(filePath, "w", true);
    if (!file) {
        logger->error("Failed to open '%s' for writing!", filePath);
        return -1;
    }

    cache.push_back(std::make_shared<Macro>(macro));

    for (const auto& macro : cache) {
        logger->info("Writing \"%s=%s\"", macro->name.c_str(), macro->content.c_str());
        file.printf("%s=%s\n", macro->name.c_str(), macro->content.c_str());
    }

    file.close();

    */

    return 0;
}

std::vector<std::shared_ptr<common::macros::Macro>> common::macros::MacroStorage::readAll() {

    return cache;
    /*
    if (!hasBegun) {
        beginResult = LittleFS.begin(false, "/data");
        if (LittleFS.exists(filePath)) {
            LittleFS.remove(filePath);
        }
        hasBegun = true;
    }

    cache.clear();

    logger->info(
        "LittleFS begin = %i. Used = %i. Total = %i.",
        beginResult,
        LittleFS.usedBytes(),
        LittleFS.totalBytes()
    );

    logger->info("Listing files");
    listDir("/");
    logger->info("Listing files complete");

    if (!hasRead) {
        auto file = LittleFS.open(filePath, "rw", false);
        if (!file) {
            logger->error("Failed to open '%s' for reading!", filePath);
            return cache;
        }

        char nameBuffer[128];
        uint nameIndex = 0;
        char contentBuffer[256];
        uint contentIndex = 0;

        int state = 1;

        while (file.available()) {
            char c = file.read();

            if (c == '=') {
                state = 2;
            } else if (c == '\n') {
                auto name = std::string(nameBuffer, nameIndex);
                auto content = std::string(contentBuffer, contentIndex);
                cache.push_back(std::make_shared<Macro>(Macro{.name = name, .content = content}));
                nameIndex = 0;
                contentIndex = 0;
                state = 1;
            } else if (state == 1) {
                nameBuffer[nameIndex++] = c;
                if (nameIndex >= 128) {
                    file.close();
                    return cache;
                }
            } else if (state == 2) {
                contentBuffer[contentIndex++] = c;
                if (contentIndex >= 256) {
                    file.close();
                    return cache;
                }
            }
        }

        file.close();

        //hasRead = true;
    }

    return cache;
    */
}

namespace {
    std::vector temp{
        std::make_shared<common::macros::Macro>("IntelliJ - Run", std::make_shared<common::macros::ShortcutMacroData>(1, 0x10, 0x43)),
        std::make_shared<common::macros::Macro>("IntelliJ - Debug", std::make_shared<common::macros::ShortcutMacroData>(2, 0x10, 0x42)),
        std::make_shared<common::macros::Macro>("IntelliJ - Stop", std::make_shared<common::macros::ShortcutMacroData>(3, 0x10, 0x3b))
    };
}

void common::macros::MacroStorage::forEach(const std::function<void(const Macro&)>& fn) {
    for (auto& macro : temp) {
        fn(*macro);
    }
}
