#include "DeviceRuntimeA.h"

#include <firmwares/common/logging/Logger.h>

#include "DeviceModuleA.h"

namespace {
    auto logger = common::logging::createLogger("DeviceRuntimeA");
}

DeviceRuntimeA::DeviceRuntimeA(
    uint64_t deviceId,
    RegisterManager& registers,
    IndicatorLedManager& indicatorLeds,
    Notifier& notifier
) : DeviceRuntime(deviceId, registers, indicatorLeds, notifier) {
    switchStateChangeNotifier = std::make_unique<SwitchStateChangeNotifier>(notifier);
}

DeviceRuntimeA::~DeviceRuntimeA() {
    for (const auto& switchMonitor : this->switchMonitors) {
        switchMonitor->onSwitchStateChanged().removeObserver(switchStateChangeNotifier.get());
    }
};

void DeviceRuntimeA::begin() {
    DeviceRuntime::begin();

    for (auto& switchMonitor : this->switchMonitors) {
        switchMonitor->begin();
        switchMonitor->onSwitchStateChanged().addObserver(switchStateChangeNotifier.get());
    }

    indicatorLeds.begin();
}

void DeviceRuntimeA::loop() {
    DeviceRuntime::loop();

    for (const auto& switchMonitor : this->switchMonitors) {
        switchMonitor->update();
    }

    indicatorLeds.update();
}
