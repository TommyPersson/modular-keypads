#include "DeviceModuleFactoryA.h"

#include <firmwares/common/runtimes/RegisterRefresher.h>
#include <firmwares/common/runtimes/RemoteRegisterRefresher.h>

#include "DeviceModuleA.h"
#include "DeviceRuntimeA.h"
#include "LocalRegisterRefresherA.h"

using namespace devices;
using namespace devices::a;

DeviceModuleFactoryA::~DeviceModuleFactoryA() = default;

std::unique_ptr<DeviceModule> DeviceModuleFactoryA::createLocal(
    DeviceConfiguration& config,
    ServiceLocator& serviceLocator
) {
    auto registers = std::make_unique<utils::registers::RegisterManager>();

    auto indicatorLeds = IndicatorLedManager::NeoPixel(12, 6);

    std::unique_ptr<RegisterRefresher> registerRefresher = std::make_unique<LocalRegisterRefresherA>(*registers);

    auto notifier = serviceLocator.notifierFactory.create(config.id);

    std::unique_ptr<DeviceRuntime> runtime = std::make_unique<DeviceRuntimeA>(
        config.id,
        *registers,
        *indicatorLeds,
        *notifier,
        serviceLocator.deviceConfigurationManager
    );

    return std::make_unique<DeviceModuleA>(
        DeviceMode::Local,
        config,
        indicatorLeds,
        registers,
        registerRefresher,
        runtime,
        notifier,
        serviceLocator.i2cClient
    );
}

std::unique_ptr<DeviceModule> DeviceModuleFactoryA::createRemote(
    DeviceConfiguration& config,
    ServiceLocator& serviceLocator
) {
    auto registers = std::make_unique<utils::registers::RegisterManager>();

    auto indicatorLeds = IndicatorLedManager::NoOp(12);

    std::unique_ptr<RegisterRefresher> registerRefresher = std::make_unique<RemoteRegisterRefresher>(
        *registers,
        serviceLocator.i2cClient,
        config.address,
        registers::all.size()
    );

    auto notifier = serviceLocator.notifierFactory.create(config.id);

    std::unique_ptr<DeviceRuntime> runtime = std::make_unique<DeviceRuntimeA>(
        config.id,
        *registers,
        *indicatorLeds,
        *notifier,
        serviceLocator.deviceConfigurationManager
    );

    return std::make_unique<DeviceModuleA>(
        DeviceMode::Remote,
        config,
        indicatorLeds,
        registers,
        registerRefresher,
        runtime,
        notifier,
        serviceLocator.i2cClient
    );
}

bool DeviceModuleFactoryA::matches(char deviceType) {
    return deviceType == 'a' || deviceType == 'A';
}

utils::i2c::Pins DeviceModuleFactoryA::getI2cPins() {
    return i2c::pins;
}
