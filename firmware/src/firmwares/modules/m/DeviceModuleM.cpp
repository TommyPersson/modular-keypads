#include "DeviceModuleM.h"

#include "DeviceRuntimeM.h"
#include "LocalRegisterRefresherM.h"
#include "utils/logging/Logger.h"

namespace {
    auto logger = utils::logging::createLogger("DeviceModuleM");
}

using namespace devices::m;

DeviceModuleM::DeviceModuleM(
    const DeviceConfiguration& configuration,
    std::unique_ptr<IndicatorLedManager>& indicatorLedManager,
    std::unique_ptr<RegisterManager>& registerManager,
    std::unique_ptr<RegisterRefresher>& registerRefresher,
    std::unique_ptr<DeviceRuntime>& deviceRuntime,
    std::unique_ptr<Notifier>& notifier
) :
    configuration(configuration),
    indicatorLedManager(std::move(indicatorLedManager)),
    registerManager(std::move(registerManager)),
    registerRefresher(std::move(registerRefresher)),
    deviceRuntime(std::move(deviceRuntime)),
    notifier(std::move(notifier)) {}

DeviceModuleM::~DeviceModuleM() = default;

void DeviceModuleM::setup() {
    indicatorLedManager->begin();
    registerRefresher->begin();
    deviceRuntime->begin();
}

void DeviceModuleM::loop() {
    registerRefresher->loop();
    deviceRuntime->loop();
    indicatorLedManager->update();
}

RegisterManager& DeviceModuleM::getRegisters() {
    return *registerManager;
}
