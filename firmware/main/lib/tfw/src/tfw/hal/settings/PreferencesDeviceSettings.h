#pragma once

#include "DeviceSettings.h"

#ifdef ARDUINO

#include <Preferences.h>

namespace tfw::hal::settings {
    /**
     * Stores settings in NVS using Arduino Preferences. Values aren't cached, wrap it in CachedDeviceSettings for that.
     *
     * The namespace is kept open from begin() onwards.
     */
    class PreferencesDeviceSettings final : public DeviceSettings {
    public:
        // The namespace name is limited to 15 characters
        explicit PreferencesDeviceSettings(const char* namespaceName);
        ~PreferencesDeviceSettings() override;

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
        const char* namespaceName;
        Preferences preferences;
        bool isStarted = false;
    };
}

#endif // ARDUINO
