#pragma once

#include "DeviceConfigurationManager.h"
#include "i2c/I2cClient.h"

class DeviceProxy {
public:
    explicit DeviceProxy(const DeviceConfiguration& configuration, I2cClient& i2c);
    ~DeviceProxy();

    DeviceConfiguration& getConfiguration();

    std::array<uint8_t, 32>& readRegisters();

private:
    DeviceConfiguration configuration;
    std::array<uint8_t, 32> registerData{};
    I2cClient& i2c;
};
