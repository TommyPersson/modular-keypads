#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

namespace tfw::hal::settings {
    // NVS stores keys in a 16 byte field, including the null terminator
    inline constexpr size_t MAX_KEY_LENGTH = 15;

    /**
     * Persistent key-value storage for device settings, modelled after Arduino Preferences.
     *
     * Keys are limited to MAX_KEY_LENGTH characters, as with Preferences.
     */
    class DeviceSettings {
    public:
        virtual ~DeviceSettings() = default;

        virtual bool begin() = 0;

        virtual bool isKey(std::string_view key) = 0;
        virtual bool remove(std::string_view key) = 0;

        virtual uint32_t getUInt(std::string_view key, uint32_t defaultValue) = 0;
        virtual bool putUInt(std::string_view key, uint32_t value) = 0;

        virtual int32_t getInt(std::string_view key, int32_t defaultValue) = 0;
        virtual bool putInt(std::string_view key, int32_t value) = 0;

        virtual bool getBool(std::string_view key, bool defaultValue) = 0;
        virtual bool putBool(std::string_view key, bool value) = 0;
    };
}
