#include "KeyBindingStorage.h"

#include <LittleFS.h>

#include <tfw/hal/files.h>
#include <tfw/utils/strings.h>
#include <tfw/utils/allocations.h>

#include <tfw/hal/logging.h>

using namespace common::keybindings;

namespace {
    auto logger = tfw::hal::logging::createLogger("KeyBindingStorage");

    auto filePath = "/data/key-bindings.txt";
    auto tempFilePath = "/data/key-bindings.txt.tmp";
}

namespace {
    std::shared_ptr<KeyBinding> deserializeKeyBinding(const std::string_view& line, tfw::utils::allocations::Arena& arena) {
        tfw::utils::allocations::ArenaAllocator<std::string_view> stringViewAllocator(arena);
        tfw::utils::allocations::ArenaAllocator<KeyBinding> keyBindingAllocator(arena);
        tfw::utils::allocations::ArenaAllocator<PushButtonTrigger> pushButtonTriggerAllocator(arena);
        tfw::utils::allocations::ArenaAllocator<RotaryEncoderTrigger> rotaryEncoderTriggerAllocator(arena);

        auto parts = tfw::utils::allocations::arena::strings::split(line, ':', stringViewAllocator, 10);

        auto type = static_cast<TriggerType>(tfw::utils::strings::atol(parts[0], 16));
        auto deviceId = tfw::utils::strings::atou64(parts[1], 16);

        if (type == PUSH_BUTTON) {
            auto number = tfw::utils::strings::atol(parts[2], 16);
            auto macroId = tfw::utils::strings::atou16(parts[3], 16);

            return std::allocate_shared<KeyBinding>(
                keyBindingAllocator,
                std::allocate_shared<PushButtonTrigger>(pushButtonTriggerAllocator, deviceId, number),
                macroId
            );
        }

        if (type == ROTARY_ENCODER) {
            auto number = tfw::utils::strings::atol(parts[2], 16);
            auto direction = static_cast<RotationalEncoderDirection>(tfw::utils::strings::atol(parts[3], 16));
            auto macroId = tfw::utils::strings::atou16(parts[4], 16);

            return std::allocate_shared<KeyBinding>(
                keyBindingAllocator,
                std::allocate_shared<RotaryEncoderTrigger>(rotaryEncoderTriggerAllocator, deviceId, number, direction),
                macroId
            );
        }

        return nullptr;
    }

    std::string_view serializeKeyBinding(const KeyBinding& keyBinding, tfw::utils::allocations::Arena& arena) {
        if (keyBinding.trigger->type == PUSH_BUTTON) {
            const auto& trigger = dynamic_cast<PushButtonTrigger&>(*keyBinding.trigger);
            return tfw::utils::allocations::arena::strings::sprintf(
                arena,
                "0x%02x:%08llx:0x%02x:0x%04x",
                trigger.type,
                trigger.deviceId,
                trigger.number,
                keyBinding.macroId
            );
        }

        if (keyBinding.trigger->type == ROTARY_ENCODER) {
            const auto& trigger = dynamic_cast<RotaryEncoderTrigger&>(*keyBinding.trigger);
            return tfw::utils::allocations::arena::strings::sprintf(
                arena,
                "0x%02x:%08llx:0x%02x:0x%02x:0x%04x",
                trigger.type,
                trigger.deviceId,
                trigger.number,
                trigger.direction,
                keyBinding.macroId
            );
        }

        return "";
    }
}

void KeyBindingStorage::setup() {
    if (!LittleFS.begin(false)) {
        logger->error("Unable to initialize file system");
    }
}

error_t KeyBindingStorage::write(const KeyBinding& keyBinding) {
    remove(*keyBinding.trigger);

    fs::File outputFile = LittleFS.open(filePath, "a", true);
    if (!outputFile) {
        logger->error("Failed to open '%s'", tempFilePath);
        return -1;
    }

    tfw::utils::allocations::Arena arena(512);

    auto serializedKeyBinding = serializeKeyBinding(keyBinding, arena);

    outputFile.println(serializedKeyBinding.data());

    outputFile.close();

    onKeyBindingSetSubject.notify({});

    return 0;
}

error_t KeyBindingStorage::remove(const Trigger& trigger) {
    fs::File tempOutputFile = LittleFS.open(tempFilePath, "w", true);
    if (!tempOutputFile) {
        logger->error("Failed to open '%s'", tempFilePath);
        return -1;
    }

    tfw::utils::allocations::Arena arena(512);

    forEach([&](const KeyBinding& keyBinding) {
        if (*keyBinding.trigger != trigger) {
            const auto serializedKeyBinding = serializeKeyBinding(keyBinding, arena);
            tempOutputFile.println(serializedKeyBinding.data());
        }

        arena.reset();
    });

    tempOutputFile.close();

    LittleFS.remove(filePath);
    LittleFS.rename(tempFilePath, filePath);

    onKeyBindingClearedSubject.notify({});

    return 0;
}

error_t KeyBindingStorage::removeAll(const uint16_t& macroId) {
    fs::File tempOutputFile = LittleFS.open(tempFilePath, "w", true);
    if (!tempOutputFile) {
        logger->error("Failed to open '%s'", tempFilePath);
        return -1;
    }

    tfw::utils::allocations::Arena arena(512);

    forEach([&](const KeyBinding& keyBinding) {
        if (keyBinding.macroId != macroId) {
            const auto serializedKeyBinding = serializeKeyBinding(keyBinding, arena);
            tempOutputFile.println(serializedKeyBinding.data());
        }

        arena.reset();
    });

    tempOutputFile.close();

    LittleFS.remove(filePath);
    LittleFS.rename(tempFilePath, filePath);

    onKeyBindingClearedSubject.notify({});

    return 0;
}

void KeyBindingStorage::forEach(const std::function<void(const KeyBinding&)>& callback) {
    tfw::utils::allocations::Arena arena(1024);

    const auto rc = tfw::utils::files::iterateLines(
        filePath,
        [&](const std::string_view& line) {
            const auto macro = deserializeKeyBinding(line, arena);
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
