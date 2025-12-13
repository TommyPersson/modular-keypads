#pragma once

#include <memory>

#include "DeviceProxy.h"
#include "i2c/I2cClient.h"

class DeviceScanner {
public:
    explicit DeviceScanner(I2cClient& client);
    ~DeviceScanner();

    std::vector<std::shared_ptr<DeviceProxy>> scan();

private:
    I2cClient& client;
};
