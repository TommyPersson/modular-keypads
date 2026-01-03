#pragma once

#include <memory>

#include "DeviceProxy.h"
#include "utils/i2c/Client.h"

class DeviceScanner {
public:
    explicit DeviceScanner(utils::i2c::Client& client);
    ~DeviceScanner();

    std::vector<std::shared_ptr<DeviceProxy>> scan();

private:
    utils::i2c::Client& client;
};
