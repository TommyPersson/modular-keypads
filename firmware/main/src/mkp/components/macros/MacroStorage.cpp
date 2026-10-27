#include "MacroStorage.h"

#include <tfw/utils/strings.h>
#include <tfw/utils/allocations.h>
#include <tfw/hal/fs.h>

#include <tfw/hal/logging.h>

#include "MacroDataSerializers.h"

using namespace mkp::components::macros;

namespace {
    auto logger = tfw::hal::logging::createLogger("MacroStorage");
    auto filePath = "/data/macro-definitions.txt";
    auto tempFilePath = "/data/macro-definitions.txt.tmp";
}

namespace {
    // The ID of a stored line, or 0 for lines that can't be macros at all
    uint16_t storedMacroId(const std::span<const std::string_view>& parts) {
        if (parts.size() < 3) {
            return 0;
        }

        return tfw::utils::strings::atou16(parts[0]);
    }

    std::shared_ptr<Macro> deserializeStoredMacro(
        const std::span<const std::string_view>& parts,
        tfw::utils::allocations::Arena& arena
    ) {
        tfw::utils::allocations::ArenaAllocator<Macro> macroAllocator(arena);

        // Lines that can't be parsed are ignored
        const auto macroId = storedMacroId(parts);
        if (macroId == 0) {
            return nullptr;
        }

        auto name = parts[1];
        auto typePart = parts[2];
        auto type = static_cast<MacroType>(tfw::utils::strings::atol(typePart, 16));

        std::shared_ptr<MacroData> macroData = nullptr;

        for (auto serializer : macroDataSerializers) {
            auto typedSerializer = static_cast<MacroDataStorageSerializer<MacroData>*>(serializer);
            if (typedSerializer->handles(type)) {
                macroData = typedSerializer->deserialize(macroId, parts.subspan(3), arena);
            }
        }

        if (macroData != nullptr) {
            return std::allocate_shared<Macro>(macroAllocator, name, macroData);
        }

        return nullptr;
    }

    std::string_view serializeStoredMacro(const Macro& macro, tfw::utils::allocations::Arena& arena) {
        std::string_view dataPart;

        for (auto serializer : macroDataSerializers) {
            auto typedSerializer = static_cast<MacroDataStorageSerializer<MacroData>*>(serializer);
            if (typedSerializer->handles(macro.data->type)) {
                dataPart = typedSerializer->serialize(*macro.data, arena);
            }
        }

        if (!dataPart.empty()) {
            return tfw::utils::allocations::arena::strings::sprintf(
                arena,
                "%d:%.*s:0x%02x:%.*s",
                macro.data->id,
                static_cast<int>(macro.name.length()),
                macro.name.data(),
                static_cast<uint8_t>(macro.data->type),
                static_cast<int>(dataPart.length()),
                dataPart.data()
            );
        }

        return "";
    }

    // Stored lines with their IDs, and their macros when they can be parsed
    int iterateStoredLines(
        tfw::hal::fs::FileSystem& fileSystem,
        const std::function<void(const std::string_view&, uint16_t, const Macro*)>& callback
    ) {
        tfw::utils::allocations::Arena arena(1024);

        return tfw::hal::fs::iterateLines(
            fileSystem,
            filePath,
            [&](const std::string_view& line) {
                tfw::utils::allocations::ArenaAllocator<std::string_view> stringViewAllocator(arena);
                const auto parts = tfw::utils::allocations::arena::strings::split(line, ':', stringViewAllocator, 10);

                const auto macro = deserializeStoredMacro(parts, arena);
                callback(line, storedMacroId(parts), macro.get());

                arena.reset();
            }
        );
    }

    // Lines of macros that can't be parsed, such as ones of types added in later firmware, are written back as they
    // were, so that saving with older firmware doesn't lose them
    void writeStoredLine(
        tfw::hal::fs::File& file,
        const std::string_view& line,
        const Macro* macro,
        tfw::utils::allocations::Arena& arena
    ) {
        const auto serialized = macro != nullptr
            ? serializeStoredMacro(*macro, arena)
            : tfw::utils::allocations::arena::strings::sprintf(
                arena,
                "%.*s",
                static_cast<int>(line.length()),
                line.data()
            );

        if (!serialized.empty()) {
            file.println(serialized.data());
        }
    }
}

MacroStorage::MacroStorage(tfw::hal::fs::FileSystem& fileSystem) : fileSystem(fileSystem) {

}

void MacroStorage::setup() {
}

int MacroStorage::write(const Macro& macro) {
    const auto tempOutputFile = fileSystem.open(tempFilePath, tfw::hal::fs::FileMode::Write, true);
    if (!tempOutputFile) {
        logger->error("Failed to open '%s'", tempFilePath);
        return -1;
    }

    tfw::utils::allocations::Arena arena(2048);

    uint16_t highestIdSeen = 0;
    bool macroFound = false;

    iterateStoredLines(fileSystem, [&](const std::string_view& line, const uint16_t id, const Macro* storedMacro) {
        // Lines that can't be macros at all are dropped
        if (id == 0) {
            return;
        }

        if (id == macro.data->id) {
            // Overwrite with new macro
            writeStoredLine(*tempOutputFile, line, &macro, arena);
            macroFound = true;
        } else {
            // Re-write existing macro, in the current format
            writeStoredLine(*tempOutputFile, line, storedMacro, arena);
        }

        highestIdSeen = std::max(highestIdSeen, id);

        arena.reset();
    });

    // Write the new macro if it wasn't found in existing macros
    if (!macroFound) {
        if (macro.data->id <= 0) {
            macro.data->id = highestIdSeen + 1;
        }
        auto serializedMacro = serializeStoredMacro(macro, arena);
        tempOutputFile->println(serializedMacro.data());
    }

    tempOutputFile->close();

    fileSystem.remove(filePath);
    fileSystem.rename(tempFilePath, filePath);

    numStored.reset();

    onMacroSavedSubject.notify({.macroId = macro.data->id});

    return 0;
}

int MacroStorage::remove(uint16_t id) {
    const auto tempOutputFile = fileSystem.open(tempFilePath, tfw::hal::fs::FileMode::Write, true);
    if (!tempOutputFile) {
        logger->error("Failed to open '%s'", tempFilePath);
        return -1;
    }

    tfw::utils::allocations::Arena arena(2048);

    iterateStoredLines(fileSystem, [&](const std::string_view& line, const uint16_t storedId, const Macro* storedMacro) {
        if (storedId != 0 && storedId != id) {
            writeStoredLine(*tempOutputFile, line, storedMacro, arena);
        }

        arena.reset();
    });

    tempOutputFile->close();

    fileSystem.remove(filePath);
    fileSystem.rename(tempFilePath, filePath);

    numStored.reset();

    onMacroRemovedSubject.notify({.macroId = id});

    return 0;
}

void MacroStorage::forEach(const std::function<void(const Macro&)>& callback) {
    const auto rc = iterateStoredLines(fileSystem, [&](const std::string_view&, uint16_t, const Macro* macro) {
        if (macro != nullptr) {
            callback(*macro);
        }
    });

    if (rc != 0) {
        logger->error("Failed to read '%s' (%i)", filePath, rc);
    }
}

uint64_t MacroStorage::count() {
    uint64_t num = 0;
    forEach([&](const Macro&) {
        num += 1;
    });
    return num;
}

uint64_t MacroStorage::getNumStored() {
    if (!numStored.has_value()) {
        numStored = count();
    }

    return numStored.value();
}
