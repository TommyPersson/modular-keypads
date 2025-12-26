#include "MacroStorage.h"

#include <Preferences.h>
#include <FS.h>
#include <LittleFS.h>

#include "utils/strings.h"
#include "utils/allocations/Arena.h"
#include "utils/allocations/ArenaUtils.h"
#include "utils/files.h"

#include "firmwares/common/logging/Logger.h"

using namespace common::macros;

namespace {
    auto logger = common::logging::createLogger("MacroStorage");
    bool beginResult = false;
    bool hasBegun = false;
    bool hasRead = false;
    Preferences prefs;
    auto filePath = "/data/macro-definitions.txt";
}

namespace {
    std::shared_ptr<Macro> deserializeStoredMacro(const std::string_view& line, Arena& arena) {
        ArenaAllocator<std::string_view> stringViewAllocator(arena);

        auto parts = arena::strings::split(line, ':', stringViewAllocator, 10);

        auto idPart = parts[0];
        auto namePart = parts[1];
        auto name = std::string(namePart); // TODO avoid allocation
        auto typePart = parts[2];
        auto type = static_cast<MacroType>(utils::strings::atol(typePart, 16));

        if (type == SHORTCUT) {
            auto dataPart = parts[3];
            auto dataParts = arena::strings::split(dataPart, ',', stringViewAllocator, 2);
            auto modifiersPart = dataParts[0];
            auto hidCodePart = dataParts[1];

            return std::make_shared<Macro>(
                name,
                std::make_shared<ShortcutMacroData>(
                    utils::strings::atol(idPart, 10),
                    utils::strings::atol(modifiersPart, 16),
                    utils::strings::atol(hidCodePart, 16)
                )
            );
        }

        // TODO the sequence type

        return nullptr;
    }
}

// TODO do we need to be careful with allocations here?
void MacroStorage::setup() {
}

error_t MacroStorage::write(const Macro& macro) {
    if (!hasBegun) {
        beginResult = LittleFS.begin(false);
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

void MacroStorage::forEach(const std::function<void(const Macro&)>& callback) {
    if (!hasBegun) {
        beginResult = LittleFS.begin(false);
        hasBegun = true;
    }

    logger->debug(
        "LittleFS begin = %i. Used = %i. Total = %i.",
        beginResult,
        LittleFS.usedBytes(),
        LittleFS.totalBytes()
    );

    Arena arena(1024);

    const auto rc = utils::files::iterateLines(
        filePath,
        [&](const std::string_view& line) {
            const auto macro = deserializeStoredMacro(line, arena);
            if (macro != nullptr) {
                callback(*macro);
            }
        }
    );

    if (rc != 0) {
        logger->error("Failed to read '%s' (%i)", filePath, rc);
    }
}
