#pragma once

#include <firmwares/modules/common/DeviceModuleFactory.h>

namespace devices::a {

class DeviceModuleFactoryA final : public devices::DeviceModuleFactory {
public:
    ~DeviceModuleFactoryA() override;
    std::unique_ptr<DeviceModule> createLocal(DeviceConfiguration& config, ServiceLocator& serviceLocator) override;
    std::unique_ptr<DeviceModule> createRemote(DeviceConfiguration& config, ServiceLocator& serviceLocator) override;
    bool matches(char deviceType) override;
    utils::i2c::Pins getI2cPins() override;
};

}
