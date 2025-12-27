#include "MacroStorage.h"

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
    auto filePath = "/data/macro-definitions.txt";
    auto tempFilePath = "/data/macro-definitions.txt.tmp";
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

            auto macro = std::make_shared<Macro>(
                name,
                std::make_shared<ShortcutMacroData>(
                    utils::strings::atol(idPart, 10),
                    utils::strings::atol(modifiersPart, 16),
                    utils::strings::atol(hidCodePart, 16)
                )
            );

            return macro;
        }

        // TODO the sequence type

        return nullptr;
    }

    std::string_view serializeStoredMacro(const Macro& macro, Arena& arena) {
        const auto& shortcutData = std::dynamic_pointer_cast<ShortcutMacroData>(macro.data);
        if (shortcutData != nullptr) {
            return arena::strings::sprintf(
                arena,
                "%i:%s:0x%02x:0x%02x,0x%02x",
                shortcutData->id,
                macro.name.c_str(),
                shortcutData->type,
                shortcutData->modifiers,
                shortcutData->hidKeyCode
            );
        } else {
            // TODO the sequence type
            return "";
        }
    }
}

void MacroStorage::setup() {
}

error_t MacroStorage::write(const Macro& macro) {
    if (!hasBegun) {
        if (!LittleFS.begin(false)) {
            return -2;
        }
        hasBegun = true;
    }

    fs::File tempOutputFile = LittleFS.open(tempFilePath, "w", true);
    if (!tempOutputFile) {
        logger->error("Failed to open '%s'", tempFilePath);
        return -1;
    }

    Arena arena(2048);

    uint16_t highestIdSeen = 0;

    forEach([&](const Macro& storedMacro) {
        auto serializedMacro = storedMacro.data->id == macro.data->id
            ? serializeStoredMacro(macro, arena) // Overwrite with new
            : serializeStoredMacro(storedMacro, arena); // Re-write existing

        highestIdSeen = std::max(highestIdSeen, storedMacro.data->id);

        tempOutputFile.println(serializedMacro.data());

        arena.reset();
    });

    if (macro.data->id <= 0) {
        macro.data->id = highestIdSeen + 1;
        auto serializedMacro = serializeStoredMacro(macro, arena);
        tempOutputFile.println(serializedMacro.data());
    }

    tempOutputFile.close();

    LittleFS.remove(filePath);
    LittleFS.rename(tempFilePath, filePath);

    return 0;
}

void MacroStorage::forEach(const std::function<void(const Macro&)>& callback) {
    if (!hasBegun) {
        if (!LittleFS.begin(false)) {
            return;
        }
        hasBegun = true;
    }

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
