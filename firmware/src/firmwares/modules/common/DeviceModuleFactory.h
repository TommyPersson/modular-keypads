#pragma once

#include <memory>

#include "DeviceModule.h"

namespace devices {
    class DeviceModuleFactory {
    public:
        virtual ~DeviceModuleFactory() = default;

        virtual std::unique_ptr<DeviceModule> createLocal(
            DeviceConfiguration& config,
            ServiceLocator& serviceLocator
        ) = 0;

        virtual std::unique_ptr<DeviceModule> createRemote(
            DeviceConfiguration& config,
            ServiceLocator& serviceLocator
        ) = 0;

        virtual bool matches(char deviceType) = 0;

        virtual utils::i2c::Pins getI2cPins() = 0;

    protected:
        DeviceModuleFactory() = default;
    };
}
