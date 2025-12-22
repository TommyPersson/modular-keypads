#include "DeviceRuntimeM.h"

#include <firmwares/common/DeviceCapabilities.h>

#include "DeviceModuleM.h"
#include "RegisterDescriptorsM.h"

DeviceRuntimeM::DeviceRuntimeM(
    uint64_t deviceId,
    RegisterManager& registers,
    IndicatorLedManager& indicatorLeds,
    Notifier& notifier
) : DeviceRuntime(deviceId, registers, indicatorLeds, notifier) {
    switchStateChangeNotifier = std::make_unique<SwitchStateChangeNotifier>(notifier);
    encoderRotationNotifier = std::make_unique<EncoderRotationNotifier>(notifier);

    configureRegister(devices::m::registers::IOA);
    configureRegister(devices::m::registers::IOB);
}

DeviceRuntimeM::~DeviceRuntimeM() {
    for (const auto& switchMonitor : this->switchMonitors) {
        switchMonitor->onSwitchStateChanged().removeObserver(switchStateChangeNotifier.get());
    }

    for (const auto& encoderMonitor : this->rotationalEncoderMonitors) {
        encoderMonitor->onEncoderRotated().removeObserver(encoderRotationNotifier.get());
    }
}

void DeviceRuntimeM::begin() {
    DeviceRuntime::begin();

    for (const auto& switchMonitor : this->switchMonitors) {
        switchMonitor->begin();
        switchMonitor->onSwitchStateChanged().addObserver(switchStateChangeNotifier.get());
    }

    for (const auto& encoderMonitor : this->rotationalEncoderMonitors) {
        encoderMonitor->begin();
        encoderMonitor->onEncoderRotated().addObserver(encoderRotationNotifier.get());
    }

    indicatorLeds.begin();
}

void DeviceRuntimeM::loop() {
    DeviceRuntime::loop();

    for (const auto& switchMonitor : this->switchMonitors) {
        switchMonitor->update();
    }

    for (const auto& encoderMonitor : this->rotationalEncoderMonitors) {
        encoderMonitor->update();
    }

    indicatorLeds.update();
}
