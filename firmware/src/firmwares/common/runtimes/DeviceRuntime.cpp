#include "DeviceRuntime.h"

#include <firmwares/modules/m/DeviceModuleM.h>

using namespace devices;

DeviceRuntime::DeviceRuntime(
    uint64_t deviceId,
    RegisterManager& registers,
    IndicatorLedManager& indicatorLeds,
    Notifier& notifier
) : deviceId(deviceId),
    registers(registers),
    indicatorLeds(indicatorLeds),
    notifier(notifier) {
}

void DeviceRuntime::configureRegisters() const {
    for (const auto& descriptor : getRegisterDescriptors()) {
        registers.configure(*descriptor);
    }
}

void DeviceRuntime::configureCapabilities() {
    const auto capabilities = getCapabilities();

    for (const auto& capability : capabilities) {
        if (
            const auto pushButton = dynamic_cast<PushButtonCapability*>(capability.get());
            pushButton != nullptr
        ) {
            const auto& reg = registers.get(pushButton->reg);

            attachSwitch(pushButton->number, BitReader::forRegister(*reg, pushButton->regIndex), pushButton->ledIndex);
        }

        if (
            const auto rotaryEncoder = dynamic_cast<RotaryEncoderCapability*>(capability.get());
            rotaryEncoder != nullptr
        ) {
            const auto& reg = registers.get(rotaryEncoder->reg);

            // TODO support inversion in capability?
            attachRotationalEncoder(
                rotaryEncoder->number,
                BitReader::forRegister(*reg, rotaryEncoder->aRegIndex, BitReaderMode::Inverted),
                BitReader::forRegister(*reg, rotaryEncoder->bRegIndex, BitReaderMode::Inverted)
            );
        }
    }
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
    auto rotaryEncoderMonitor = std::make_shared<RotationalEncoderMonitor>(number, aBitReader, bBitReader);
    rotaryEncoderMonitor->onEncoderRotated().addObserver(this);
    this->rotationalEncoderMonitors.emplace_back(rotaryEncoderMonitor);
}

void DeviceRuntime::begin() {
    configureRegisters();
    configureCapabilities();
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

void DeviceRuntime::observe(const EncoderRotatedEvent& event) {
    auto deviceRotaryEncoderEvent = DeviceRotaryEncoderEvent{
        .deviceId = deviceId,
        .encoderNumber = event.encoderNumber,
        .direction = event.direction,
    };

    deviceRotaryEncoderEventSubject.notify(deviceRotaryEncoderEvent);
}
