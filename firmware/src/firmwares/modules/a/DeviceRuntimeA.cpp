#include "DeviceRuntimeA.h"

#include <tfw/hal/logging.h>

#include "DeviceModuleA.h"

namespace {
    auto logger = tfw::hal::logging::createLogger("DeviceRuntimeA");
}

DeviceRuntimeA::DeviceRuntimeA(
    uint64_t deviceId,
    tfw::utils::registers::RegisterManager& registers,
    IndicatorLedManager& indicatorLeds,
    Notifier& notifier,
    DeviceConfigurationManager& configurationManager
) : DeviceRuntime(deviceId, registers, indicatorLeds, notifier, configurationManager) {
    switchStateChangeNotifier = std::make_unique<SwitchStateChangeNotifier>(notifier);
}

DeviceRuntimeA::~DeviceRuntimeA() {
    for (const auto& button : this->buttons) {
        button->onStateChanged().removeObserver(switchStateChangeNotifier.get());
    }
};

void DeviceRuntimeA::begin() {
    DeviceRuntime::begin();

    for (const auto& button : this->buttons) {
        button->begin();
        button->onStateChanged().addObserver(switchStateChangeNotifier.get());
    }

    indicatorLeds.begin();
}

void DeviceRuntimeA::loop() {
    DeviceRuntime::loop();

    for (const auto& button : this->buttons) {
        button->update();
    }

    indicatorLeds.update();
}
