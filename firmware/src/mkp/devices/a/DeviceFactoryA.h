#pragma once

#include "mkp/devices/common/DeviceFactory.h"

namespace devices::a {

class DeviceFactoryA final : public devices::DeviceFactory {
public:
    ~DeviceFactoryA() override;
    bool matches(char deviceType) override;
    std::unique_ptr<LocalDevice> createLocal(DeviceConfiguration& config, ServiceLocator& serviceLocator) override;
    std::unique_ptr<RemoteDevice> createRemote(DeviceConfiguration& config, ServiceLocator& serviceLocator) override;
};

}
