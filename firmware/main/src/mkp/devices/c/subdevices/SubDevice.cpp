#include "SubDevice.h"

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

    constexpr uint8_t DEVICE_TYPE_SMC1 = 0x01;
    constexpr uint8_t DEVICE_TYPE_SMC2 = 0x02;

    // The SMC reports its own button events, so there are no registers. The LED indices are the SMC's own (see
    // the button definitions in the SMC firmwares), the SMC maps button numbers to them.
    const std::vector<std::shared_ptr<DeviceCapability>> smc1Capabilities = {
        std::make_shared<PushButtonCapability>(1, NoRegister, 0, 0),
        std::make_shared<PushButtonCapability>(2, NoRegister, 0, 1),
        std::make_shared<PushButtonCapability>(3, NoRegister, 0, 3),
        std::make_shared<PushButtonCapability>(4, NoRegister, 0, 2),
    };

    const std::vector<std::shared_ptr<DeviceCapability>> smc2Capabilities = {
        std::make_shared<PushButtonCapability>(1, NoRegister, 0, 0),
        std::make_shared<PushButtonCapability>(2, NoRegister, 0, 1),
        std::make_shared<PushButtonCapability>(3, NoRegister, 0, 2),
        std::make_shared<PushButtonCapability>(4, NoRegister, 0, 5),
        std::make_shared<PushButtonCapability>(5, NoRegister, 0, 4),
        std::make_shared<PushButtonCapability>(6, NoRegister, 0, 3),
    };

    const std::vector<std::shared_ptr<DeviceCapability>> noCapabilities = {};

    const std::vector<std::shared_ptr<DeviceCapability>>& capabilitiesFor(const uint8_t deviceType) {
        switch (deviceType) {
        case DEVICE_TYPE_SMC1: return smc1Capabilities;
        case DEVICE_TYPE_SMC2: return smc2Capabilities;
        default: return noCapabilities;
        }
    }
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
    pinINT(pinINT),
    capabilities(&noCapabilities) {

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
    return *capabilities;
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

    // The SMC stores the name asynchronously, so reading it back here could still return the old one
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
            capabilities = &noCapabilities;
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
    capabilities = &capabilitiesFor(deviceType);

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
            }

            if (event[0] == 1) {
                scanScheduled = true;
            }
        }
    }

    eventsAvailable = !pinINT.read();
}
