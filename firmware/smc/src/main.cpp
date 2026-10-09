#include <tinyNeoPixel.h>

#include "firmwares/Firmware.h"
#include "firmwares/FirmwareFactory.h"
#include "hal/identity/DeviceId.h"
#include "hal/identity/DeviceName.h"
#include "hal/spi/SpiSlave.h"

namespace {
    auto firmwareFactory = smc::firmwares::FirmwareFactory(
        {
            .pinQ = PIN_PB3,
            .pinCLK = PIN_PB6,
            .pinCE = PIN_PB4,
            .pinLD = PIN_PB5,
        }
    );

    smc::hal::spi::SpiSlave spiSlave;
    smc::hal::identity::DeviceNameStorage deviceNameStorage;
    smc::firmwares::Firmware* firmware = nullptr;
    smc::events::EventQueue eventQueue;

    uint32_t prevUs = 0;
}

void setup() {
    Serial.swap(1);
    Serial.begin(9600);

    firmware = firmwareFactory.create();
    firmware->setup();

    uint8_t deviceId[smc::hal::identity::DEVICE_ID_LENGTH];
    smc::hal::identity::computeDeviceId(deviceId);
    spiSlave.setDeviceId(deviceId);

    uint8_t deviceName[smc::hal::identity::DEVICE_NAME_LENGTH];
    deviceNameStorage.load(deviceName);
    spiSlave.setDeviceName(deviceName);
    Serial.printf("Device name: %s\r\n", reinterpret_cast<const char*>(deviceName));

    spiSlave.setup();
    spiSlave.setDeviceType(firmware->deviceType());
    spiSlave.enqueueEvent({.type = smc::events::types::BOOTED});
}


void processCommand(const smc::hal::spi::Command& command) {
    switch (command.type) {
    case smc::hal::spi::commands::FLASH_IDENTIFICATION_LIGHTS:
        firmware->flashIdentificationLights();
        break;
    case smc::hal::spi::commands::FLASH_BUTTON_IDENTIFICATION_LIGHT:
        firmware->flashButtonIdentificationLight(command.arguments[0]);
        break;
    case smc::hal::spi::commands::WRITE_DEVICE_NAME: {
        uint8_t deviceName[smc::hal::identity::DEVICE_NAME_LENGTH];
        memcpy(deviceName, command.arguments, sizeof(deviceName));

        // Stored as given, but served normalized, the same as it will be after a restart
        deviceNameStorage.store(deviceName);
        smc::hal::identity::normalizeDeviceName(deviceName);
        spiSlave.setDeviceName(deviceName);
        break;
    }
    default:
        break;
    }
}

void loop() {
    delayMicroseconds(500);

    const auto currentUs = micros();
    const auto deltaUs = currentUs - prevUs;
    prevUs = currentUs;

    spiSlave.update(deltaUs, eventQueue);

    smc::hal::spi::Command command;
    while (spiSlave.dequeueCommand(command)) {
        processCommand(command);
    }

    firmware->update(deltaUs, eventQueue);

    if (deviceNameStorage.update()) {
        eventQueue.enqueue({.type = smc::events::types::DEVICE_NAME_STORED});
    }

    while (const auto* event = eventQueue.dequeue()) {
        spiSlave.enqueueEvent(*event);

        Serial.printf("%i %i\n\r", event->type, event->data);
    }
}
