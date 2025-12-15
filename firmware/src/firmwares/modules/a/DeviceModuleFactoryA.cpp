#include "DeviceModuleFactoryA.h"

#include <firmwares/common/runtimes/RegisterRefresher.h>
#include <firmwares/common/runtimes/RemoteRegisterRefresher.h>

#include "DeviceModuleA.h"
#include "DeviceRuntimeA.h"
#include "LocalRegisterRefresherA.h"

using namespace devices::a;

DeviceModuleFactoryA::~DeviceModuleFactoryA() = default;

std::unique_ptr<DeviceModule> DeviceModuleFactoryA::createLocal(
    DeviceConfiguration& config,
    ServiceLocator& serviceLocator
    ) {
    auto registers = std::make_unique<RegisterManager>();

    auto indicatorLeds = IndicatorLedManager::NeoPixel(12, 6);

    std::unique_ptr<RegisterRefresher> registerRefresher = std::make_unique<LocalRegisterRefresherA>(*registers);

    std::unique_ptr<DeviceRuntime> runtime = std::make_unique<DeviceRuntimeA>(
        *registers,
        *indicatorLeds,
        serviceLocator.notifier
        );

    return std::make_unique<DeviceModuleA>(config, indicatorLeds, registers, registerRefresher, runtime);
}

std::unique_ptr<DeviceModule> DeviceModuleFactoryA::createRemote(
    DeviceConfiguration& config,
    ServiceLocator& serviceLocator
    ) {
    auto registers = std::make_unique<RegisterManager>();

    auto indicatorLeds = IndicatorLedManager::NoOp(12);

    std::unique_ptr<RegisterRefresher> registerRefresher = std::make_unique<RemoteRegisterRefresher>(
        *registers, serviceLocator.i2cClient, config.address
        );

    // TODO configure notifier with correct ID

    std::unique_ptr<DeviceRuntime> runtime = std::make_unique<DeviceRuntimeA>(
        *registers,
        *indicatorLeds,
        serviceLocator.notifier
        );

    return std::make_unique<DeviceModuleA>(config, indicatorLeds, registers, registerRefresher, runtime);
}

bool DeviceModuleFactoryA::matches(char deviceType) {
    return deviceType == 'a' || deviceType == 'A';
}

::i2c::Pins DeviceModuleFactoryA::getI2cPins() {
    return i2c::pins;
}
