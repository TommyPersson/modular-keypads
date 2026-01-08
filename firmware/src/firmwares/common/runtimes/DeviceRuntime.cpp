#include "DeviceRuntime.h"

#include <firmwares/modules/m/DeviceModuleM.h>

using namespace devices;
using namespace utils;

DeviceRuntime::DeviceRuntime(
    uint64_t deviceId,
    utils::registers::RegisterManager& registers,
    IndicatorLedManager& indicatorLeds,
    Notifier& notifier,
    DeviceConfigurationManager& configurationManager
) : deviceId(deviceId),
    registers(registers),
    indicatorLeds(indicatorLeds),
    notifier(notifier),
    configurationManager(configurationManager) {
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

            attachSwitch(pushButton->number, bitreaders::for_register(*reg, pushButton->regIndex),
                         pushButton->ledIndex);
        }

        if (
            const auto rotaryEncoder = dynamic_cast<RotaryEncoderCapability*>(capability.get());
            rotaryEncoder != nullptr
        ) {
            const auto& reg = registers.get(rotaryEncoder->reg);

            // TODO support inversion in capability?
            attachRotationalEncoder(
                rotaryEncoder->number,
                bitreaders::for_register(*reg, rotaryEncoder->aRegIndex, bitreaders::Mode::Inverted),
                bitreaders::for_register(*reg, rotaryEncoder->bRegIndex, bitreaders::Mode::Inverted)
            );
        }
    }
}


void DeviceRuntime::attachSwitch(uint8_t number, const std::shared_ptr<utils::bitreaders::BitReader>& bitReader,
                                 int8_t ledIndex) {
    const auto switchMonitor = this->switchMonitors.emplace_back(std::make_shared<SwitchMonitor>(number, bitReader));

    switchMonitor->onSwitchStateChanged().addObserver(this);

    if (ledIndex < 0) {
        return;
    }

    indicatorLeds.connectToSwitch(ledIndex, *switchMonitor);
}

void DeviceRuntime::attachRotationalEncoder(
    const uint8_t number,
    const std::shared_ptr<utils::bitreaders::BitReader>& aBitReader,
    const std::shared_ptr<utils::bitreaders::BitReader>& bBitReader
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

void DeviceRuntime::flashIdentificationLights(uint32_t durationMs) {
    const auto capabilities = getCapabilities();

    for (const auto& capability : capabilities) {
        if (const auto pushButton = dynamic_cast<PushButtonCapability*>(capability.get()); pushButton != nullptr) {
            if (pushButton->ledIndex >= 0) {
                const auto indicatorLed = indicatorLeds.get(pushButton->ledIndex);
                if (indicatorLed) {
                    auto color = indicatorLed->makeColor(255, 0, 0, 0);
                    indicatorLed->animate(utils::led::animations::pulse(color, durationMs));
                }
            }
        }
    }
}

void DeviceRuntime::flashButtonIdentificationLight(uint8_t buttonNumber, uint32_t durationMs) {
    const auto capabilities = getCapabilities();

    for (const auto& capability : capabilities) {
        if (const auto pushButton = dynamic_cast<PushButtonCapability*>(capability.get()); pushButton != nullptr) {
            if (pushButton->number != buttonNumber) {
                continue;
            }

            if (pushButton->ledIndex >= 0) {
                const auto indicatorLed = indicatorLeds.get(pushButton->ledIndex);
                if (indicatorLed) {
                    auto color = indicatorLed->makeColor(255, 0, 0, 0);
                    indicatorLed->animate(utils::led::animations::pulse(color, durationMs));
                }
            }
        }
    }
}

utils::void_result DeviceRuntime::renameDevice(const std::string_view& deviceName) {
    if (!configurationManager.setDeviceName(deviceName)) {
        return void_result::error("unable.to.set.device.name");
    }

    return void_result::success();
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
