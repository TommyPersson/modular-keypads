#include "DeviceModuleFactoryM.h"

#include <firmwares/common/runtimes/RegisterRefresher.h>
#include <firmwares/common/runtimes/RemoteRegisterRefresher.h>

#include "DeviceModuleM.h"
#include "LocalRegisterRefresherM.h"

using namespace devices::m;

DeviceModuleFactoryM::~DeviceModuleFactoryM() = default;

std::unique_ptr<DeviceModule> DeviceModuleFactoryM::createLocal(
    DeviceConfiguration& config,
    ServiceLocator& serviceLocator
    ) {
    auto registers = std::make_unique<RegisterManager>();

    auto indicatorLeds = IndicatorLedManager::NeoPixel(5, 7);

    std::unique_ptr<RegisterRefresher> registerRefresher = std::make_unique<LocalRegisterRefresherM>(*registers);

    std::unique_ptr<DeviceRuntime> runtime = std::make_unique<DeviceRuntimeM>(
        *registers,
        *indicatorLeds,
        serviceLocator.notifier
        );

    return std::make_unique<DeviceModuleM>(config, indicatorLeds, registers, registerRefresher, runtime);
}

std::unique_ptr<DeviceModule> DeviceModuleFactoryM::createRemote(
    DeviceConfiguration& config,
    ServiceLocator& serviceLocator
    ) {
    auto registers = std::make_unique<RegisterManager>();

    auto indicatorLeds = IndicatorLedManager::NoOp(5);

    std::unique_ptr<RegisterRefresher> registerRefresher = std::make_unique<RemoteRegisterRefresher>(
        *registers, serviceLocator.i2cClient, config.address
        );

    // TODO configure notifier with correct ID

    std::unique_ptr<DeviceRuntime> runtime = std::make_unique<DeviceRuntimeM>(
        *registers,
        *indicatorLeds,
        serviceLocator.notifier
        );

    return std::make_unique<DeviceModuleM>(config, indicatorLeds, registers, registerRefresher, runtime);
}

bool DeviceModuleFactoryM::matches(char deviceType) {
    return deviceType == 'm' || deviceType == 'M';
}

::i2c::Pins DeviceModuleFactoryM::getI2cPins() {
    return i2c::pins;
}
