#pragma once

#include <memory>

#include "DeviceConfigurationManager.h"
#include <tfw/hal/i2c.h>

namespace mkp::devices::common {
    class DeviceScanner {
    public:
        explicit DeviceScanner(tfw::hal::i2c::Client& client);
        ~DeviceScanner();

        std::vector<std::shared_ptr<DeviceConfiguration>> scan();

    private:
        tfw::hal::i2c::Client& client;
    };
}
