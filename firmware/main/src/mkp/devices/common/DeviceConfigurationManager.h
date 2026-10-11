#pragma once

#include <string>
#include <Preferences.h>

#include "DeviceTypeDetector.h"

namespace mkp::devices::common {
    struct DeviceConfiguration {
        uint64_t id;
        std::string name;
        std::string type;
        uint8_t address;
        uint64_t parentDeviceId;
        // Position of a sub-device on its parent, starting at 1, or 0 for devices that aren't sub-devices
        uint8_t subDeviceSlot = 0;
    };

    class DeviceConfigurationManager {
    public:
        explicit DeviceConfigurationManager(
            Preferences& preferences,
            DeviceTypeDetector& deviceTypeDetector
        );

        ~DeviceConfigurationManager() = default;

        void begin() const;

        uint64_t getDeviceId() const;
        std::string getDeviceVersion() const;

        std::string getDeviceName() const;
        bool setDeviceName(const std::string_view& deviceName) const;

        std::string getDeviceType() const;

        DeviceConfiguration getDeviceConfiguration() const;

    private:
        Preferences& preferences;
        DeviceTypeDetector& deviceTypeDetector;
    };
}