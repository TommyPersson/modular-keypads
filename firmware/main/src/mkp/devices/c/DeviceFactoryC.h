#pragma once

#include "mkp/devices/common/DeviceFactory.h"

namespace mkp::devices::c {
    class DeviceFactoryC final : public common::DeviceFactory {
    public:
        ~DeviceFactoryC() override;
        bool matches(char deviceType) override;

        std::unique_ptr<common::LocalDevice> createLocal(
            common::DeviceConfiguration& config,
            firmwares::base::ServiceLocator& serviceLocator
        ) override;

        std::unique_ptr<common::RemoteDevice> createRemote(
            common::DeviceConfiguration& config,
            firmwares::base::ServiceLocator& serviceLocator
        ) override;
    };
}
