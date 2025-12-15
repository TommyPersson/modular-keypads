#include "DeviceRuntime.h"

DeviceRuntime::DeviceRuntime(
    RegisterManager& registers,
    IndicatorLedManager& indicatorLeds,
    Notifier& notifier
    ) :
    registers(registers),
    indicatorLeds(indicatorLeds),
    notifier(notifier) {
}

void DeviceRuntime::attachSwitch(uint8_t number, const std::shared_ptr<BitReader>& bitReader, uint8_t ledIndex) {
    const auto switchMonitor = this->switchMonitors.emplace_back(std::make_shared<SwitchMonitor>(number, bitReader));
    indicatorLeds.connectToSwitch(ledIndex, *switchMonitor);
}

void DeviceRuntime::attachRotationalEncoder(
    const uint8_t number,
    const std::shared_ptr<BitReader>& aBitReader,
    const std::shared_ptr<BitReader>& bBitReader
    ) {
    this->rotationalEncoderMonitors.emplace_back(
        std::make_shared<RotationalEncoderMonitor>(number, aBitReader, bBitReader)
        );
}


std::shared_ptr<Register> DeviceRuntime::configureRegister(const RegisterDescriptor& descriptor) const {
    return this->registers.configure(descriptor);
}

void DeviceRuntime::begin() {
}

void DeviceRuntime::loop() {
}
