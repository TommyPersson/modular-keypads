#include "DeviceFactoryA.h"

#include "LocalDeviceA.h"

using namespace mkp::devices::common;
using namespace mkp::devices::a;
using namespace mkp::firmwares::base;

DeviceFactoryA::~DeviceFactoryA() = default;

bool DeviceFactoryA::matches(const std::string_view deviceType) {
    return deviceType == "a" || deviceType == "A";
}

std::unique_ptr<LocalDevice> DeviceFactoryA::createLocal(
    DeviceConfiguration& config,
    ServiceLocator& serviceLocator
) {
    return std::make_unique<LocalDeviceA>(
        config,
        serviceLocator.deviceConfigurationManager,
        serviceLocator.clock
    );
}

std::unique_ptr<RemoteDevice> DeviceFactoryA::createRemote(
    DeviceConfiguration& config,
    ServiceLocator& serviceLocator
) {
    return std::make_unique<RemoteDevice>(
        config,
        capabilities,
        serviceLocator.i2cClient
    );
}
