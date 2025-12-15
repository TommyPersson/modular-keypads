#pragma once

#include <firmwares/modules/common/DeviceModuleFactory.h>

#include "DeviceRuntimeM.h"

namespace devices::m {

class DeviceModuleFactoryM final : public devices::common::DeviceModuleFactory {
public:
    ~DeviceModuleFactoryM() override;
    std::unique_ptr<DeviceModule> createLocal(DeviceConfiguration& config, ServiceLocator& serviceLocator) override;
    std::unique_ptr<DeviceModule> createRemote(DeviceConfiguration& config, ServiceLocator& serviceLocator) override;
    bool matches(char deviceType) override;
    ::i2c::Pins getI2cPins() override;
};

}
