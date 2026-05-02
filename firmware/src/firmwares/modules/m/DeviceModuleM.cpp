#include "DeviceModuleM.h"

#include <firmwares/slave/i2c/commands/FlashDeviceIdentificationLightsRemoteCommandHandler.h>

#include "DeviceRuntimeM.h"
#include "LocalRegisterRefresherM.h"
#include <tfw/hal/logging.h>

namespace {
    auto logger = tfw::utils::logging::createLogger("DeviceModuleM");
}

using namespace devices::m;

DeviceModuleM::DeviceModuleM(
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

DeviceModuleM::~DeviceModuleM() = default;

void DeviceModuleM::setup() {
    indicatorLedManager->begin();
    registerRefresher->setup();
    deviceRuntime->begin();
}

void DeviceModuleM::loop() {
    registerRefresher->loop();
    deviceRuntime->loop();
    indicatorLedManager->update();
}

tfw::utils::registers::RegisterManager& DeviceModuleM::getRegisters() {
    return *registerManager;
}
