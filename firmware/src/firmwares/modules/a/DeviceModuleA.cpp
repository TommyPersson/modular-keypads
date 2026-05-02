#include "DeviceModuleA.h"

#include <firmwares/slave/i2c/commands/FlashDeviceIdentificationLightsRemoteCommandHandler.h>

#include <tfw/hal/logging.h>

namespace {
    auto logger = tfw::utils::logging::createLogger("DeviceModuleA");
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
    tfw::utils::i2c::Client& i2cClient
) : DeviceModule(deviceLocation, i2cClient),
    configuration(configuration),
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
}

void DeviceModuleA::loop() {
    registerRefresher->loop();
    deviceRuntime->loop();
    indicatorLedManager->update();
}

tfw::utils::registers::RegisterManager& DeviceModuleA::getRegisters() {
    return *registerManager;
}
