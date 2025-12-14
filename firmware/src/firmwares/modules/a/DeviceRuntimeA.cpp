#include "DeviceRuntimeA.h"

DeviceRuntimeA::DeviceRuntimeA(
    RegisterManager& registers,
    IndicatorLedManager& indicatorLeds,
    Notifier& notifier,
    const DeviceMode mode
    ) :
    DeviceRuntime(registers, indicatorLeds, notifier, mode) {

    switchStateChangeNotifier = std::make_unique<SwitchStateChangeNotifier>(notifier);

    const auto& ioaReg = this->configureRegister(devices::a::registers::IOA);
    const auto& iobReg = this->configureRegister(devices::a::registers::IOB);

    attachSwitch(1, BitReader::forRegister(*iobReg, 0), 0);
    attachSwitch(2, BitReader::forRegister(*iobReg, 1), 11);
    attachSwitch(3, BitReader::forRegister(*iobReg, 2), 1);
    attachSwitch(4, BitReader::forRegister(*iobReg, 3), 10);
    attachSwitch(5, BitReader::forRegister(*iobReg, 4), 2);
    attachSwitch(6, BitReader::forRegister(*iobReg, 5), 9);
    attachSwitch(7, BitReader::forRegister(*iobReg, 6), 3);
    attachSwitch(8, BitReader::forRegister(*iobReg, 7), 8);

    attachSwitch(9, BitReader::forRegister(*ioaReg, 0), 4);
    attachSwitch(10, BitReader::forRegister(*ioaReg, 1), 7);
    attachSwitch(11, BitReader::forRegister(*ioaReg, 2), 5);
    attachSwitch(12, BitReader::forRegister(*ioaReg, 3), 6);
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
