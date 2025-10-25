#pragma once

#include <string>
#include <Preferences.h>

#include "logging/Logger.h"

class DeviceConfigurationManager {
public:
    explicit DeviceConfigurationManager(Preferences& preferences, Logger& logger);
    ~DeviceConfigurationManager() = default;

    void begin() const;

    std::string getDeviceId() const;
    std::string getDeviceVersion() const;

    uint8_t getDeviceAddress() const;
    bool setDeviceAddress(uint8_t deviceAddress) const;

    std::string getDeviceName() const;
    bool setDeviceName(const std::string_view& deviceName) const;

    char getDeviceType() const;
    bool setDeviceType(char deviceType) const;

private:
    Preferences& preferences;
    Logger& logger;
};
