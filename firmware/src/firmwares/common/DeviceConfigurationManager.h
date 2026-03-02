#pragma once

#include <string>
#include <Preferences.h>

#include "DeviceTypeDetector.h"

struct DeviceConfiguration {
    uint64_t id;
    std::string name;
    char type;
    uint8_t address;
};

class DeviceConfigurationManager {
public:
    explicit DeviceConfigurationManager(
        Preferences& preferences,
        devices::DeviceTypeDetector& deviceTypeDetector
    );

    ~DeviceConfigurationManager() = default;

    void begin() const;

    uint64_t getDeviceId() const;
    std::string getDeviceVersion() const;

    uint8_t getDeviceAddress() const;
    bool setDeviceAddress(uint8_t deviceAddress) const;

    std::string getDeviceName() const;
    bool setDeviceName(const std::string_view& deviceName) const;

    char getDeviceType() const;

    DeviceConfiguration getDeviceConfiguration() const;

private:
    Preferences& preferences;
    devices::DeviceTypeDetector& deviceTypeDetector;
};
