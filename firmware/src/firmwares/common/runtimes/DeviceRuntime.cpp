#include "DeviceRuntime.h"

DeviceRuntime::DeviceRuntime(
    RegisterManager& registers,
    IndicatorLedManager& indicatorLeds,
    Notifier& notifier,
    Logger& logger,
    const DeviceMode mode
    ) :
    registers(registers),
    indicatorLeds(indicatorLeds),
    notifier(notifier),
    logger(logger),
    mode(mode) {
}

void DeviceRuntime::attachSwitch(uint8_t number, const std::shared_ptr<BitReader>& bitReader, uint8_t ledIndex) {
    const auto switchMonitor = this->switchMonitors.emplace_back(std::make_shared<SwitchMonitor>(number, bitReader));
    indicatorLeds.connectToSwitch(ledIndex, *switchMonitor);
}

std::shared_ptr<Register> DeviceRuntime::addRegister(const std::string& name) const {
    return this->registers.add(name);
}

void DeviceRuntime::begin() {
}

void DeviceRuntime::loop() {
}
