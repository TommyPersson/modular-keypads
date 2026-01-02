#include "DeviceModuleA.h"

#include "utils/logging/Logger.h"

namespace {
    auto logger = utils::logging::createLogger("DeviceModuleA");
}

using namespace devices::a;

DeviceModuleA::DeviceModuleA(
    const DeviceConfiguration& configuration,
    std::unique_ptr<IndicatorLedManager>& indicatorLedManager,
    std::unique_ptr<utils::registers::RegisterManager>& registerManager,
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

DeviceModuleA::~DeviceModuleA() = default;

void DeviceModuleA::setup() {
    indicatorLedManager->begin();
    registerRefresher->begin();
    deviceRuntime->begin();
}

void DeviceModuleA::loop() {
    registerRefresher->loop();
    deviceRuntime->loop();
    indicatorLedManager->update();
}

utils::registers::RegisterManager& DeviceModuleA::getRegisters() {
    return *registerManager;
}
