#pragma once

#include <map>
#include <string>
#include <variant>

#include "DeviceSettings.h"

namespace tfw::hal::settings {
    /**
     * Caches the values of another DeviceSettings, so that reading a value only reaches the underlying storage the
     * first time. Writes go straight through to the underlying storage.
     */
    class CachedDeviceSettings final : public DeviceSettings {
    public:
        explicit CachedDeviceSettings(DeviceSettings& backing);

        bool begin() override;

        bool isKey(std::string_view key) override;
        bool remove(std::string_view key) override;

        uint32_t getUInt(std::string_view key, uint32_t defaultValue) override;
        bool putUInt(std::string_view key, uint32_t value) override;

        int32_t getInt(std::string_view key, int32_t defaultValue) override;
        bool putInt(std::string_view key, int32_t value) override;

        bool getBool(std::string_view key, bool defaultValue) override;
        bool putBool(std::string_view key, bool value) override;

    private:
        // std::monostate marks keys that are known not to exist
        using CachedValue = std::variant<std::monostate, uint32_t, int32_t, bool>;

        template <typename T, typename TReader>
        T get(std::string_view key, T defaultValue, TReader readBacking);

        void store(std::string_view key, bool written, CachedValue value);

        DeviceSettings& backing;
        std::map<std::string, CachedValue, std::less<>> cache;
    };
}
