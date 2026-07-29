#pragma once

#include <memory>

#include "mkp/firmwares/base/ServiceLocator.h"
#include "LocalDevice.h"
#include "RemoteDevice.h"

namespace mkp::devices::common {
    class DeviceFactory {
    public:
        virtual ~DeviceFactory() = default;

        virtual std::unique_ptr<LocalDevice> createLocal(
            DeviceConfiguration& config,
            firmwares::base::ServiceLocator& serviceLocator
        ) = 0;

        virtual std::unique_ptr<RemoteDevice> createRemote(
            DeviceConfiguration& config,
            firmwares::base::ServiceLocator& serviceLocator
        ) = 0;

        virtual bool matches(char deviceType) = 0;

    protected:
        DeviceFactory() = default;
    };
}
