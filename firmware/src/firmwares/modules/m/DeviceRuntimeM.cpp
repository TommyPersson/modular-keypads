#include "DeviceRuntimeM.h"

#include "DeviceModuleM.h"

DeviceRuntimeM::DeviceRuntimeM(
    uint64_t deviceId,
    tfw::utils::registers::RegisterManager& registers,
    IndicatorLedManager& indicatorLeds,
    Notifier& notifier,
    DeviceConfigurationManager& configurationManager
) : DeviceRuntime(deviceId, registers, indicatorLeds, notifier, configurationManager) {
    switchStateChangeNotifier = std::make_unique<SwitchStateChangeNotifier>(notifier);
    encoderRotationNotifier = std::make_unique<EncoderRotationNotifier>(notifier);
}

DeviceRuntimeM::~DeviceRuntimeM() {
    for (const auto& switchMonitor : this->buttons) {
        switchMonitor->onStateChanged().removeObserver(switchStateChangeNotifier.get());
    }

    for (const auto& encoderMonitor : this->rotationalEncoderMonitors) {
        encoderMonitor->onEncoderRotated().removeObserver(encoderRotationNotifier.get());
    }
}

void DeviceRuntimeM::begin() {
    DeviceRuntime::begin();

    for (const auto& button : this->buttons) {
        button->begin();
        button->onStateChanged().addObserver(switchStateChangeNotifier.get());
    }

    for (const auto& encoderMonitor : this->rotationalEncoderMonitors) {
        encoderMonitor->begin();
        encoderMonitor->onEncoderRotated().addObserver(encoderRotationNotifier.get());
    }

    indicatorLeds.begin();
}

void DeviceRuntimeM::loop() {
    DeviceRuntime::loop();

    for (const auto& button : this->buttons) {
        button->update();
    }

    for (const auto& encoderMonitor : this->rotationalEncoderMonitors) {
        encoderMonitor->update();
    }

    indicatorLeds.update();
}
