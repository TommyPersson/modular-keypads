#include "DeviceModuleM.h"

#include <firmwares/slave/i2c/commands/EnableIdentificationLightsCommandHandler.h>

#include "DeviceRuntimeM.h"
#include "LocalRegisterRefresherM.h"
#include "utils/logging/Logger.h"

namespace {
    auto logger = utils::logging::createLogger("DeviceModuleM");
}

using namespace devices::m;

DeviceModuleM::DeviceModuleM(
    const DeviceMode deviceMode,
    const DeviceConfiguration& configuration,
    std::unique_ptr<IndicatorLedManager>& indicatorLedManager,
    std::unique_ptr<utils::registers::RegisterManager>& registerManager,
    std::unique_ptr<RegisterRefresher>& registerRefresher,
    std::unique_ptr<DeviceRuntime>& deviceRuntime,
    std::unique_ptr<Notifier>& notifier,
    utils::i2c::Client& i2cClient
) : deviceMode(deviceMode),
    configuration(configuration),
    indicatorLedManager(std::move(indicatorLedManager)),
    registerManager(std::move(registerManager)),
    registerRefresher(std::move(registerRefresher)),
    deviceRuntime(std::move(deviceRuntime)),
    notifier(std::move(notifier)),
    i2cClient(i2cClient) {
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

utils::registers::RegisterManager& DeviceModuleM::getRegisters() {
    return *registerManager;
}

void DeviceModuleM::flashIdentificationLights(uint32_t durationMs) {
    if (deviceMode == DeviceMode::Remote) {
        i2cClient.sendCommand(
            // TODO do directly in (uart) command handler to reduce duplication. or DeviceModule base method?
            configuration.address,
            firmwares::slave::i2c::commands::EnableIdentificationLights,
            {.durationMs = durationMs}
        );
    } else {
        deviceRuntime->flashIdentificationLights(durationMs);
    }
}
