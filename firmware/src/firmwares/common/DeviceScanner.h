#pragma once

#include <memory>

#include "DeviceConfigurationManager.h"
#include "utils/i2c/Client.h"

class DeviceScanner {
public:
    explicit DeviceScanner(utils::i2c::Client& client);
    ~DeviceScanner();

    std::vector<std::shared_ptr<DeviceConfiguration>> scan();

private:
    utils::i2c::Client& client;
};
