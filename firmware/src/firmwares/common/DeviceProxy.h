#pragma once

#include "DeviceConfigurationManager.h"
#include "utils/i2c/Client.h"

class DeviceProxy {
public:
    explicit DeviceProxy(const DeviceConfiguration& configuration, utils::i2c::Client& i2c);
    ~DeviceProxy();

    DeviceConfiguration& getConfiguration();

    std::array<uint8_t, 32>& readRegisters();

private:
    DeviceConfiguration configuration;
    std::array<uint8_t, 32> registerData{};
    utils::i2c::Client& i2c;
};
