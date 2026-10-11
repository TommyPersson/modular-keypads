#pragma once

#include <array>
#include <cstdint>
#include <string_view>

#include <tfw/hal/settings.h>

namespace mkp::components::settings {
    struct UIntSetting {
        // Limited to 15 characters, and can't contain ':' as it's used as a separator when listing settings
        const char* key;
        uint32_t defaultValue;
        uint32_t minValue;
        uint32_t maxValue;

        uint32_t get(tfw::hal::settings::DeviceSettings& deviceSettings) const {
            return deviceSettings.getUInt(key, defaultValue);
        }

        bool isValid(const uint32_t value) const {
            return value >= minValue && value <= maxValue;
        }
    };

    // Time between characters typed by text macros. Some applications drop characters when typing too fast.
    inline constexpr UIntSetting TypingDelayMs{
        .key = "typing.delay",
        .defaultValue = 1,
        .minValue = 1, // Repeated characters can be missed without a delay
        .maxValue = 1000,
    };

    // Time to keep the keyboard modifiers of mouse macros, and of shortcut macros that hold them without a hold time of
    // their own, held after the last such macro. Repeated macros, such as when scrolling fast, then hold the modifiers once, instead of tapping them
    // for each macro. Zero releases them right after each macro.
    inline constexpr UIntSetting ModifierHoldMs{
        .key = "modifier.hold",
        .defaultValue = 250,
        .minValue = 0,
        .maxValue = 5000,
    };

    inline constexpr std::array AllUIntSettings{
        TypingDelayMs,
        ModifierHoldMs,
    };

    consteval bool isValidKey(const std::string_view key) {
        return !key.empty() && key.length() <= tfw::hal::settings::MAX_KEY_LENGTH && key.find(':') == std::string_view::npos;
    }

    consteval bool allKeysAreValid() {
        for (const auto& setting : AllUIntSettings) {
            if (!isValidKey(setting.key)) {
                return false;
            }
        }

        return true;
    }

    static_assert(allKeysAreValid(), "Setting keys must be 1 to 15 characters, and can't contain ':'");

    inline const UIntSetting* findUIntSetting(const std::string_view key) {
        for (const auto& setting : AllUIntSettings) {
            if (setting.key == key) {
                return &setting;
            }
        }

        return nullptr;
    }
}
