#pragma once

#include <memory>
#include <string_view>

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

        virtual bool matches(std::string_view deviceType) = 0;

    protected:
        DeviceFactory() = default;
    };
}
