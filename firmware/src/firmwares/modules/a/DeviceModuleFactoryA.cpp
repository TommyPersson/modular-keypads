#include "DeviceModuleFactoryA.h"

#include <firmwares/common/runtimes/RemoteRegisterRefresher.h>

#include "../../../mkp/devices/a/LocalDeviceA.h"

using namespace devices;
using namespace devices::a;

DeviceModuleFactoryA::~DeviceModuleFactoryA() = default;

std::unique_ptr<LocalDevice> DeviceModuleFactoryA::createLocal(
    DeviceConfiguration& config,
    ServiceLocator& serviceLocator
) {
    return std::make_unique<LocalDeviceA>(config, serviceLocator.deviceConfigurationManager, serviceLocator.notifierFactory);
}

bool DeviceModuleFactoryA::matches(char deviceType) {
    return deviceType == 'a' || deviceType == 'A';
}

tfw::hal::i2c::Pins DeviceModuleFactoryA::getI2cPins() {
    return i2c::pins;
}
