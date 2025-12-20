#include "DeviceRuntime.h"

using namespace devices;

DeviceRuntime::DeviceRuntime(
    uint64_t deviceId,
    RegisterManager& registers,
    IndicatorLedManager& indicatorLeds,
    Notifier& notifier
)
    : deviceId(deviceId)
    ,
    registers(registers)
    ,
    indicatorLeds(indicatorLeds)
    ,
    notifier(notifier) {
}

void DeviceRuntime::attachSwitch(uint8_t number, const std::shared_ptr<BitReader>& bitReader, int8_t ledIndex) {
    const auto switchMonitor = this->switchMonitors.emplace_back(std::make_shared<SwitchMonitor>(number, bitReader));

    switchMonitor->onSwitchStateChanged().addObserver(this);

    if (ledIndex < 0) {
        return;
    }

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

void DeviceRuntime::observe(const SwitchEvent& event) {
    auto deviceSwitchEvent = DeviceSwitchEvent{
        .deviceId = deviceId,
        .switchNumber = event.switchNumber,
        .state = event.state,
    };

    deviceSwitchEventSubject.notify(deviceSwitchEvent);
}
