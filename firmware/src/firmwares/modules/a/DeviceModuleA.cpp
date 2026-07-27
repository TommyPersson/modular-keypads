#include "DeviceModuleA.h"

#include <firmwares/slave/i2c/commands/FlashDeviceIdentificationLightsRemoteCommandHandler.h>

#include <tfw/hal/logging.h>

#include "tfw/utils/strings.h"

namespace {
    auto logger = tfw::hal::logging::createLogger("DeviceModuleA");
}

using namespace devices::a;

DeviceModuleA::DeviceModuleA(
    const DeviceLocation deviceLocation,
    const DeviceConfiguration& configuration,
    std::unique_ptr<IndicatorLedManager>& indicatorLedManager,
    std::unique_ptr<tfw::utils::registers::RegisterManager>& registerManager,
    std::unique_ptr<RegisterRefresher>& registerRefresher,
    std::unique_ptr<DeviceRuntime>& deviceRuntime,
    std::unique_ptr<Notifier>& notifier,
    tfw::hal::i2c::Client& i2cClient
) : DeviceModule(configuration, deviceLocation, i2cClient),
    indicatorLedManager(std::move(indicatorLedManager)),
    registerManager(std::move(registerManager)),
    registerRefresher(std::move(registerRefresher)),
    deviceRuntime(std::move(deviceRuntime)),
    notifier(std::move(notifier)) {
}

DeviceModuleA::~DeviceModuleA() = default;

void DeviceModuleA::setup() {
    indicatorLedManager->begin();
    registerRefresher->setup();
    deviceRuntime->begin();

    DeviceModule::setup();
}

void DeviceModuleA::loop() {
    DeviceModule::loop();

    registerRefresher->loop();
    deviceRuntime->loop();
    indicatorLedManager->update();
}

tfw::utils::registers::RegisterManager& DeviceModuleA::getRegisters() {
    return *registerManager;
}
