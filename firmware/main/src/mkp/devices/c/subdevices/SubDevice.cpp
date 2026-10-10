#include "SubDevice.h"

#include "mkp/devices/common/SubDeviceCapabilities.h"

#include <Arduino.h>

using namespace mkp::devices::c;
using namespace mkp::devices::common;
using namespace mkp::devices::c::smc;
using namespace tfw::hal::gpio;
using namespace tfw::utils;
using namespace tfw::hal::time;

namespace {
    // Same as the SMC's own default, which it returns until a name is set
    constexpr auto DEFAULT_NAME = "Unnamed";
}

SubDevice::SubDevice(
    Clock& clock,
    const uint8_t address,
    const uint64_t parentDeviceId,
    const uint8_t number,
    SPIClass& spi,
    OutputPin& pinCS,
    InputPin& pinINT
) : address(address),
    parentDeviceId(parentDeviceId),
    number(number),
    client(spi, pinCS),
    pinINT(pinINT) {

    scanTimer = clock.createTimer(100'000, [this](const TimerElapsedEvent& event) {
        scan();
    });

    resetConfiguration();
}

SubDevice::~SubDevice() = default;

void SubDevice::setup() {
    client.setup();
    pinINT.setup();
    pinINT.setupInterrupt(FALLING);
    pinINT.onInterruptEvent().addObserver(this);

    scan();
}

void SubDevice::loop() {
    pollEvents();

    scanTimer->update();
}

const DeviceConfiguration& SubDevice::getConfiguration() const {
    return configuration;
}

const std::vector<std::shared_ptr<DeviceCapability>>& SubDevice::getCapabilities() const {
    return subDeviceCapabilitiesFor(configuration.type);
}

void_result SubDevice::flashIdentificationLights(uint32_t durationMs) {
    // The SMC uses a fixed duration
    uint8_t status;
    if (
        !client.transact(commands::FLASH_IDENTIFICATION_LIGHTS, &status, sizeof(status))
        || status != responses::ACCEPTED
    ) {
        return void_result::error("flash.identification.lights.failed");
    }

    return void_result::success();
}

void_result SubDevice::flashButtonIdentificationLight(const uint8_t buttonNumber, uint32_t durationMs) {
    // The SMC uses a fixed duration
    const uint8_t arguments[] = {buttonNumber};
    uint8_t status;
    if (
        !client.transact(commands::FLASH_BUTTON_IDENTIFICATION_LIGHT, arguments, &status, sizeof(status))
        || status != responses::ACCEPTED
    ) {
        return void_result::error("flash.button.identification.light.failed");
    }

    return void_result::success();
}

void_result SubDevice::rename(const std::string_view& deviceName) {
    // Null padded, and always terminated
    uint8_t arguments[smc::DEVICE_NAME_LENGTH] = {};
    const auto length = std::min(deviceName.size(), sizeof(arguments) - 1);
    memcpy(arguments, deviceName.data(), length);

    uint8_t status;
    if (
        !client.transact(commands::WRITE_DEVICE_NAME, arguments, &status, sizeof(status))
        || status != responses::ACCEPTED
    ) {
        return void_result::error("rename.failed");
    }

    // Set right away so the new name is visible immediately. The SMC applies the command on its next loop and
    // stores it asynchronously, and the stored name is read back when it reports DEVICE_NAME_STORED.
    configuration.name = length > 0 ? std::string(deviceName.substr(0, length)) : DEFAULT_NAME;

    return void_result::success();
}

void SubDevice::scan() {
    const auto result = scanIdentity();
    if (!result) {
        numMissedScans++;

        if (numMissedScans > 5) {
            numMissedScans = 0;
            auto previousDeviceId = deviceId;
            deviceId = 0;
            deviceType = 0;
            resetConfiguration();

            if (previousDeviceId != 0) {
                disconnectedSubject.notify({.subDeviceNumber = number, .subDeviceId = previousDeviceId});
            }
        }

        return;
    }

    numMissedScans = 0;

    if (deviceId == result->deviceId) {
        return;
    }

    // Only read on connect, as the name is longer than the rest of the identity
    const auto name = readName();
    if (!name) {
        // Retried on the next scan, as the device still counts as unconnected
        return;
    }

    deviceId = result->deviceId;
    deviceType = result->deviceType;

    configuration.id = deviceId;
    configuration.type = "smc" + std::to_string(deviceType);
    configuration.name = *name;

    connectedSubject.notify(
        {
            .subDeviceNumber = number,
            .subDeviceId = deviceId,
            .subDeviceType = deviceType
        }
    );
}

std::optional<SubDevice::ScanResult> SubDevice::scanIdentity() {
    ScanResult result{};

    const auto deviceIdBytes = reinterpret_cast<uint8_t*>(&result.deviceId);
    if (!client.transact(commands::READ_DEVICE_ID, deviceIdBytes, sizeof(result.deviceId))) {
        return std::nullopt;
    }

    if (!client.transact(commands::READ_DEVICE_TYPE, &result.deviceType, sizeof(result.deviceType))) {
        return std::nullopt;
    }

    return result;
}

std::optional<std::string> SubDevice::readName() {
    uint8_t name[smc::DEVICE_NAME_LENGTH];
    if (!client.transact(commands::READ_DEVICE_NAME, name, sizeof(name))) {
        return std::nullopt;
    }

    const auto chars = reinterpret_cast<const char*>(name);
    return std::string(chars, strnlen(chars, sizeof(name)));
}

void SubDevice::resetConfiguration() {
    configuration.address = address;
    configuration.parentDeviceId = parentDeviceId;
    configuration.id = 0;
    configuration.name = "";
    configuration.type = "0";
}

void SubDevice::observe(const InputPinInterruptEvent& event) {
    eventsAvailable = !pinINT.read();
}

void SubDevice::pollEvents() {
    if (eventsAvailable) {
        bool nameStored = false;
        uint8_t event[2];
        while (true) {
            auto valid = client.transact(commands::READ_EVENTS, event, sizeof(event));
            if (!valid) {
                break;
            }

            if (event[0] == 0) {
                break;
            }

            uint8_t status;
            client.transact(commands::ACK_EVENT, &status, 1);

            if (event[0] == 11) {
                auto switchNumber = event[1];
                this->deviceSwitchEventSubject.notify(
                    {
                        .deviceId = deviceId,
                        .switchNumber = switchNumber,
                        .state = tfw::hal::buttons::ButtonState::PRESSED
                    }
                );
            } else if (event[0] == 12) {
                auto switchNumber = event[1];
                this->deviceSwitchEventSubject.notify(
                    {
                        .deviceId = deviceId,
                        .switchNumber = switchNumber,
                        .state = tfw::hal::buttons::ButtonState::UNPRESSED
                    }
                );
            } else if (event[0] == 21 || event[0] == 22) {
                auto encoderNumber = event[1];
                this->deviceRotaryEncoderEventSubject.notify(
                    {
                        .deviceId = deviceId,
                        .encoderNumber = encoderNumber,
                        .direction = event[0] == 21
                                         ? tfw::hal::encoders::RotaryEncoderDirection::Clockwise
                                         : tfw::hal::encoders::RotaryEncoderDirection::CounterClockwise
                    }
                );
            }

            if (event[0] == 1) {
                scanScheduled = true;
            }

            if (event[0] == 2) {
                nameStored = true;
            }
        }

        // Read once the events are drained, as several renames could have completed
        if (nameStored) {
            refreshName();
        }
    }

    eventsAvailable = !pinINT.read();
}

void SubDevice::refreshName() {
    if (deviceId == 0) {
        return;
    }

    const auto name = readName();
    if (!name) {
        return;
    }

    configuration.name = *name;
    renamedSubject.notify({.subDeviceNumber = number, .subDeviceId = deviceId});
}
