#pragma once

#include "mkp/devices/common/DeviceFactory.h"

namespace mkp::devices::a {
    class DeviceFactoryA final : public common::DeviceFactory {
    public:
        ~DeviceFactoryA() override;
        bool matches(char deviceType) override;

        std::unique_ptr<common::LocalDevice> createLocal(
            common::DeviceConfiguration& config,
            ServiceLocator& serviceLocator
        ) override;

        std::unique_ptr<common::RemoteDevice> createRemote(
            common::DeviceConfiguration& config,
            ServiceLocator& serviceLocator
        ) override;
    };
}
