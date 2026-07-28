#include "LocalDevice.h"

#include <tfw/hal/logging.h>

#include "firmwares/common/runtimes/RegisterRefresher.h"

namespace {
    auto logger = tfw::hal::logging::createLogger("DeviceModule");
}


devices::LocalDevice::LocalDevice(
    const DeviceConfiguration& configuration,
    const DeviceConfigurationManager& configurationManager,
    const NotifierFactory& notifierFactory,
    std::unique_ptr<IndicatorLedManager>&& indicatorLedManager
) : configuration(configuration),
    configurationManager(configurationManager),
    indicatorLedManager(std::move(indicatorLedManager)),
    registerManager(std::make_unique<tfw::utils::registers::RegisterManager>()),
    notifier(notifierFactory.create(configuration.id)) {

    switchStateChangeNotifier = std::make_unique<SwitchStateChangeNotifier>(*notifier);
}

void devices::LocalDevice::setup() {
    indicatorLedManager->begin();
    setupRegisters();
    setupCapabilities();
}

void devices::LocalDevice::loop() {
    for (const auto& button : this->buttons) {
        button->update();
    }

    indicatorLedManager->update();
}

tfw::utils::registers::RegisterManager& devices::LocalDevice::getRegisters() const {
    return *registerManager;
}

void devices::LocalDevice::setupRegisters() const {
    for (const auto& descriptor : getRegisterDescriptors()) {
        registerManager->configure(*descriptor);
    }
}

void devices::LocalDevice::setupCapabilities() {
    const auto capabilities = getCapabilities();

    for (const auto& capability : capabilities) {
        if (
            const auto pushButton = dynamic_cast<PushButtonCapability*>(capability.get());
            pushButton != nullptr
        ) {
            const auto& reg = registerManager->get(pushButton->reg);

            attachSwitch(
                pushButton->number,
                tfw::hal::bitreaders::for_register(*reg, pushButton->regIndex),
                pushButton->ledIndex
            );
        }

        if (
            const auto rotaryEncoder = dynamic_cast<RotaryEncoderCapability*>(capability.get());
            rotaryEncoder != nullptr
        ) {
            const auto& reg = registerManager->get(rotaryEncoder->reg);

            // TODO support inversion in capability?
            attachRotationalEncoder(
                rotaryEncoder->number,
                tfw::hal::bitreaders::for_register(
                    *reg,
                    rotaryEncoder->aRegIndex,
                    tfw::hal::bitreaders::Mode::Inverted
                ),
                tfw::hal::bitreaders::for_register(*reg, rotaryEncoder->bRegIndex, tfw::hal::bitreaders::Mode::Inverted)
            );
        }
    }
}

tfw::utils::void_result devices::LocalDevice::rename(const std::string_view& deviceName) {
    if (!configurationManager.setDeviceName(deviceName)) {
        return tfw::utils::void_result::error("unable.to.set.device.name");
    }

    return tfw::utils::void_result::success();
}

void devices::LocalDevice::observe(const DeviceSwitchEvent& event) {
    deviceSwitchEventSubject.notify(event);
}

void devices::LocalDevice::observe(const DeviceRotaryEncoderEvent& event) {
    deviceRotaryEncoderEventSubject.notify(event);
}

void devices::LocalDevice::observe(const tfw::hal::buttons::ButtonStateChangedEvent& event) {
    const auto deviceSwitchEvent = DeviceSwitchEvent{
        .deviceId = configuration.id,
        .switchNumber = event.buttonNumber,
        .state = event.toState,
    };

    deviceSwitchEventSubject.notify(deviceSwitchEvent);
}

void devices::LocalDevice::observe(const tfw::hal::encoders::EncoderRotatedEvent& event) {
    const auto deviceRotaryEncoderEvent = DeviceRotaryEncoderEvent{
        .deviceId = configuration.id,
        .encoderNumber = event.encoderNumber,
        .direction = event.direction,
    };

    deviceRotaryEncoderEventSubject.notify(deviceRotaryEncoderEvent);
}

tfw::utils::void_result devices::LocalDevice::flashIdentificationLights(uint32_t durationMs) {
    const auto capabilities = getCapabilities();

    for (const auto& capability : capabilities) {
        if (const auto pushButton = dynamic_cast<PushButtonCapability*>(capability.get()); pushButton != nullptr) {
            if (pushButton->ledIndex >= 0) {
                const auto indicatorLed = indicatorLedManager->get(pushButton->ledIndex);
                if (indicatorLed) {
                    auto color = indicatorLed->makeColor(255, 0, 0, 0);
                    indicatorLed->animate(tfw::utils::led::animations::pulse(color, durationMs));
                }
            }
        }
    }

    return tfw::utils::void_result::success();
}

tfw::utils::void_result
devices::LocalDevice::flashButtonIdentificationLight(uint8_t buttonNumber, uint32_t durationMs) {
    const auto capabilities = getCapabilities();

    for (const auto& capability : capabilities) {
        if (const auto pushButton = dynamic_cast<PushButtonCapability*>(capability.get()); pushButton != nullptr) {
            if (pushButton->number != buttonNumber) {
                continue;
            }

            if (pushButton->ledIndex >= 0) {
                const auto indicatorLed = indicatorLedManager->get(pushButton->ledIndex);
                if (indicatorLed) {
                    auto color = indicatorLed->makeColor(255, 0, 0, 0);
                    indicatorLed->animate(tfw::utils::led::animations::pulse(color, durationMs));
                }
            }
        }
    }

    return tfw::utils::void_result::success();
}


void devices::LocalDevice::attachSwitch(
    uint8_t number,
    const std::shared_ptr<tfw::hal::bitreaders::BitReader>& bitReader,
    int8_t ledIndex
) {
    const auto button = this->buttons.emplace_back(std::make_shared<tfw::hal::buttons::Button>(number, bitReader));

    button->onStateChanged().addObserver(this);
    button->onStateChanged().addObserver(switchStateChangeNotifier.get());

    if (ledIndex < 0) {
        return;
    }

    indicatorLedManager->connectToSwitch(ledIndex, *button);
}

void devices::LocalDevice::attachRotationalEncoder(
    const uint8_t number,
    const std::shared_ptr<tfw::hal::bitreaders::BitReader>& aBitReader,
    const std::shared_ptr<tfw::hal::bitreaders::BitReader>& bBitReader
) {
    auto encoder = std::make_shared<tfw::hal::encoders::RotaryEncoder>(number, aBitReader, bBitReader);
    encoder->onEncoderRotated().addObserver(this);
    this->rotaryEncoders.emplace_back(encoder);
}
