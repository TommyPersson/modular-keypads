#include "DeviceFactoryC.h"

#include "LocalDeviceC.h"

using namespace mkp::devices::common;
using namespace mkp::devices::c;
using namespace mkp::firmwares::base;

DeviceFactoryC::~DeviceFactoryC() = default;

bool DeviceFactoryC::matches(const std::string_view deviceType) {
    return deviceType == "c" || deviceType == "C";
}

std::unique_ptr<LocalDevice> DeviceFactoryC::createLocal(
    DeviceConfiguration& config,
    ServiceLocator& serviceLocator
) {
    return std::make_unique<LocalDeviceC>(
        config,
        serviceLocator.deviceConfigurationManager,
        serviceLocator.clock
    );
}

std::unique_ptr<RemoteDevice> DeviceFactoryC::createRemote(
    DeviceConfiguration& config,
    ServiceLocator& serviceLocator
) {
    return std::make_unique<RemoteDevice>(
        config,
        capabilities,
        serviceLocator.i2cClient
    );
}
