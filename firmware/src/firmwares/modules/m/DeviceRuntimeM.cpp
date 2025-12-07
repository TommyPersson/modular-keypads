#include "DeviceRuntimeM.h"

DeviceRuntimeM::DeviceRuntimeM(
    RegisterManager& registers,
    IndicatorLedManager& indicatorLeds,
    Notifier& notifier,
    Logger& logger,
    const DeviceMode mode
    ) :
    DeviceRuntime(registers, indicatorLeds, notifier, logger, mode) {

    switchStateChangeNotifier = std::make_unique<SwitchStateChangeNotifier>(notifier);
    encoderRotationNotifier = std::make_unique<EncoderRotationNotifier>(notifier);

    const auto& ioaReg = this->addRegister("IOA");
    const auto& iobReg = this->addRegister("IOB");

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
