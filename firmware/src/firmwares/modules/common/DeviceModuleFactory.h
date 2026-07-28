#pragma once

#include <memory>

#include "../../../mkp/devices/common/LocalDevice.h"

namespace devices {
    class DeviceModuleFactory {
    public:
        virtual ~DeviceModuleFactory() = default;

        virtual std::unique_ptr<LocalDevice> createLocal(
            DeviceConfiguration& config,
            ServiceLocator& serviceLocator
        ) = 0;

        virtual bool matches(char deviceType) = 0;

        virtual tfw::hal::i2c::Pins getI2cPins() = 0;

    protected:
        DeviceModuleFactory() = default;
    };
}
