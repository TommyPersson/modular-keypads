#include "DeviceRuntimeM.h"

#include "RegisterDescriptorsM.h"

DeviceRuntimeM::DeviceRuntimeM(
    uint64_t deviceId,
    RegisterManager& registers,
    IndicatorLedManager& indicatorLeds,
    Notifier& notifier
) : DeviceRuntime(deviceId, registers, indicatorLeds, notifier) {
    switchStateChangeNotifier = std::make_unique<SwitchStateChangeNotifier>(notifier);
    encoderRotationNotifier = std::make_unique<EncoderRotationNotifier>(notifier);

    const auto& ioaReg = this->configureRegister(devices::m::registers::IOA);
    const auto& iobReg = this->configureRegister(devices::m::registers::IOB);

    this->attachSwitch(1, BitReader::forRegister(*iobReg, 0), 0);
    this->attachSwitch(2, BitReader::forRegister(*iobReg, 1), 1);
    this->attachSwitch(3, BitReader::forRegister(*iobReg, 2), 2);
    this->attachSwitch(4, BitReader::forRegister(*iobReg, 3), 3);

    this->attachSwitch(5, BitReader::forRegister(*iobReg, 4), -1);

    this->attachRotationalEncoder(
        1,
        BitReader::forRegister(*ioaReg, 0, BitReaderMode::Inverted),
        BitReader::forRegister(*ioaReg, 1, BitReaderMode::Inverted)
    );
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
