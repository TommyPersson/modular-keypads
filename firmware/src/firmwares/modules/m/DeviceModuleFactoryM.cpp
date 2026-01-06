#include "DeviceModuleFactoryM.h"

#include <firmwares/common/runtimes/RegisterRefresher.h>
#include <firmwares/common/runtimes/RemoteRegisterRefresher.h>

#include "DeviceModuleM.h"
#include "LocalRegisterRefresherM.h"

using namespace devices;
using namespace devices::m;

DeviceModuleFactoryM::~DeviceModuleFactoryM() = default;

std::unique_ptr<DeviceModule> DeviceModuleFactoryM::createLocal(
    DeviceConfiguration& config,
    ServiceLocator& serviceLocator
) {
    auto registers = std::make_unique<utils::registers::RegisterManager>();

    auto indicatorLeds = IndicatorLedManager::NeoPixel(5, 7);

    std::unique_ptr<RegisterRefresher> registerRefresher = std::make_unique<LocalRegisterRefresherM>(*registers);

    auto notifier = serviceLocator.notifierFactory.create(config.id);

    std::unique_ptr<DeviceRuntime> runtime = std::make_unique<DeviceRuntimeM>(
        config.id,
        *registers,
        *indicatorLeds,
        *notifier
    );

    return std::make_unique<DeviceModuleM>(
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

std::unique_ptr<DeviceModule> DeviceModuleFactoryM::createRemote(
    DeviceConfiguration& config,
    ServiceLocator& serviceLocator
) {
    auto registers = std::make_unique<utils::registers::RegisterManager>();

    auto indicatorLeds = IndicatorLedManager::NoOp(5);

    std::unique_ptr<RegisterRefresher> registerRefresher = std::make_unique<RemoteRegisterRefresher>(
        *registers,
        serviceLocator.i2cClient,
        config.address,
        registers::all.size()
    );

    auto notifier = serviceLocator.notifierFactory.create(config.id);

    std::unique_ptr<DeviceRuntime> runtime = std::make_unique<DeviceRuntimeM>(
        config.id,
        *registers,
        *indicatorLeds,
        *notifier
    );

    return std::make_unique<DeviceModuleM>(
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

bool DeviceModuleFactoryM::matches(char deviceType) {
    return deviceType == 'm' || deviceType == 'M';
}

utils::i2c::Pins DeviceModuleFactoryM::getI2cPins() {
    return i2c::pins;
}
