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
    auto registers = std::make_unique<tfw::utils::registers::RegisterManager>();

    auto indicatorLeds = IndicatorLedManager::NeoPixel(5, 7);

    std::unique_ptr<RegisterRefresher> registerRefresher = std::make_unique<LocalRegisterRefresherM>(*registers);

    auto notifier = serviceLocator.notifierFactory.create(config.id);

    std::unique_ptr<DeviceRuntime> runtime = std::make_unique<DeviceRuntimeM>(
        config.id,
        *registers,
        *indicatorLeds,
        *notifier,
        serviceLocator.deviceConfigurationManager
    );

    return std::make_unique<DeviceModuleM>(
        config,
        indicatorLeds,
        registers,
        registerRefresher,
        runtime,
        notifier
    );
}

bool DeviceModuleFactoryM::matches(char deviceType) {
    return deviceType == 'm' || deviceType == 'M';
}

tfw::hal::i2c::Pins DeviceModuleFactoryM::getI2cPins() {
    return i2c::pins;
}
