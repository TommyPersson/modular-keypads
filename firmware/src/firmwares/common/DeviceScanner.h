#pragma once

#include <memory>

#include "DeviceConfigurationManager.h"
#include <tfw/hal/i2c.h>

class DeviceScanner {
public:
    explicit DeviceScanner(tfw::utils::i2c::Client& client);
    ~DeviceScanner();

    std::vector<std::shared_ptr<DeviceConfiguration>> scan();

private:
    tfw::utils::i2c::Client& client;
};
