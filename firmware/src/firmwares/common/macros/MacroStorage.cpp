#include "MacroStorage.h"

#include <FS.h>
#include <LittleFS.h>

#include "utils/strings.h"
#include "utils/allocations/Arena.h"
#include "utils/allocations/ArenaUtils.h"
#include "utils/files.h"

#include "firmwares/common/logging/Logger.h"

#include "MacroDataSerializers.h"

using namespace common::macros;

namespace {
    auto logger = common::logging::createLogger("MacroStorage");
    auto filePath = "/data/macro-definitions.txt";
    auto tempFilePath = "/data/macro-definitions.txt.tmp";
}

namespace {
    std::shared_ptr<Macro> deserializeStoredMacro(const std::string_view& line, Arena& arena) {
        ArenaAllocator<std::string_view> stringViewAllocator(arena);
        ArenaAllocator<Macro> macroAllocator(arena);

        auto parts = arena::strings::split(line, ':', stringViewAllocator, 10);

        auto macroIdPart = parts[0];
        auto macroId = utils::strings::atou16(macroIdPart);
        auto namePart = parts[1];
        auto name = std::string(namePart); // TODO avoid allocation
        auto typePart = parts[2];
        auto type = static_cast<MacroType>(utils::strings::atol(typePart, 16));

        std::shared_ptr<MacroData> macroData = nullptr;

        for (auto serializer : macroDataSerializers) {
            auto typedSerializer = static_cast<MacroDataStorageSerializer<MacroData>*>(serializer);
            if (typedSerializer->handles(type)) {
                macroData = typedSerializer->deserialize(macroId, parts, arena);
            }
        }

        if (macroData != nullptr) {
            return std::allocate_shared<Macro>(macroAllocator, name, macroData);
        }

        // TODO the sequence type

        return nullptr;
    }

    std::string_view serializeStoredMacro(const Macro& macro, Arena& arena) {
        std::string_view dataPart;

        for (auto serializer : macroDataSerializers) {
            auto typedSerializer = static_cast<MacroDataStorageSerializer<MacroData>*>(serializer);
            if (typedSerializer->handles(macro.data->type)) {
                dataPart = typedSerializer->serialize(*macro.data, arena);
            }
        }

        if (!dataPart.empty()) {
            return arena::strings::sprintf(
                arena,
                "%i:%s:0x%02x:%.*s",
                macro.data->id,
                macro.name.c_str(),
                macro.data->type,
                dataPart.length(),
                dataPart.data()
            );
        }

        // TODO the sequence type
        return "";
    }
}

void MacroStorage::setup() {
    if (!LittleFS.begin(false)) {
        logger->error("Unable to initialize file system");
    }
}

error_t MacroStorage::write(const Macro& macro) {
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

    onMacroSavedSubject.notify({.macroId = macro.data->id});

    return 0;
}

error_t MacroStorage::remove(uint16_t id) {
    fs::File tempOutputFile = LittleFS.open(tempFilePath, "w", true);
    if (!tempOutputFile) {
        logger->error("Failed to open '%s'", tempFilePath);
        return -1;
    }

    Arena arena(2048);

    forEach([&](const Macro& storedMacro) {
        if (storedMacro.data->id != id) {
            auto serializedMacro = serializeStoredMacro(storedMacro, arena);
            tempOutputFile.println(serializedMacro.data());
        }

        arena.reset();
    });

    tempOutputFile.close();

    LittleFS.remove(filePath);
    LittleFS.rename(tempFilePath, filePath);

    onMacroRemovedSubject.notify({.macroId = id});

    return 0;
}

void MacroStorage::forEach(const std::function<void(const Macro&)>& callback) {
    Arena arena(1024);

    const auto rc = utils::files::iterateLines(
        filePath,
        [&](const std::string_view& line) {
            const auto macro = deserializeStoredMacro(line, arena);
            if (macro != nullptr) {
                callback(*macro);
            }

            arena.reset();
        }
    );

    if (rc != 0) {
        logger->error("Failed to read '%s' (%i)", filePath, rc);
    }
}
