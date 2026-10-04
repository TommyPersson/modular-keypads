#include "SubDevice.h"

#include <Arduino.h>

#include "tfw/hal/logging/Logger.h"

using namespace mkp::devices::c;
using namespace tfw::hal::gpio;

namespace {
    auto logger = tfw::hal::logging::createLogger("SubDevice");

    const SPISettings spiSettings(1000000, MSBFIRST, SPI_MODE0);

    // CRC-8/AUTOSAR: poly 0x2F, init 0xFF, no reflection, xorout 0xFF
    uint8_t crc8(const uint8_t command, const uint8_t* data, const size_t length) {
        uint8_t crc = 0xFF;
        auto feed = [&crc](const uint8_t byte) {
            crc ^= byte;
            for (int bit = 0; bit < 8; bit++) {
                crc = crc & 0x80 ? (uint8_t)((crc << 1) ^ 0x2F) : (uint8_t)(crc << 1);
            }
        };

        feed(command);
        for (size_t i = 0; i < length; i++) {
            feed(data[i]);
        }
        return crc ^ 0xFF;
    }

    constexpr uint8_t READ_DEVICE_ID = 0x01;
    constexpr uint8_t READ_DEVICE_TYPE = 0x02;
    constexpr uint8_t READ_EVENTS = 0x03;
    constexpr uint8_t ACK_EVENT = 0x04;
}

SubDevice::SubDevice(
    const uint8_t number,
    SPIClass& spi,
    OutputPin& pinCS,
    InputPin& pinINT
) : number(number),
    spi(spi),
    pinCS(pinCS),
    pinINT(pinINT) {
}

SubDevice::~SubDevice() = default;

void SubDevice::setup() {
    pinCS.init();
    pinCS.setHigh();
    pinINT.setup();
    pinINT.setupInterrupt(FALLING);
    pinINT.onInterruptEvent().addObserver(this);
}

void SubDevice::update() {
    pollEvents();

    //if (scanScheduled) {
    //        scanScheduled = false;
    scan();
    //    }
}

void SubDevice::scan() {
    uint32_t scannedDeviceId;
    auto valid = transact(READ_DEVICE_ID, reinterpret_cast<uint8_t*>(&scannedDeviceId), sizeof(scannedDeviceId));
    if (!valid) {
        auto previousDeviceId = deviceId;
        deviceId = 0;

        if (previousDeviceId != 0) {
            disconnectedSubject.notify({.subDeviceNumber = number, .subDeviceId = previousDeviceId});
        }

        return;
    }

    uint8_t scannedDeviceType;
    auto valid2 = transact(READ_DEVICE_TYPE, &scannedDeviceType, sizeof(scannedDeviceType));
    if (!valid2) {
        this->deviceType = 0;
        return;
    }

    auto wasUpdated = this->deviceId != scannedDeviceId;

    this->deviceId = scannedDeviceId;
    this->deviceType = scannedDeviceType;

    if (wasUpdated) {
        connectedSubject.notify(
            {
                .subDeviceNumber = number,
                .subDeviceId = deviceId,
                .subDeviceType = deviceType
            }
        );
    }
}

void SubDevice::observe(const InputPinInterruptEvent& event) {
    eventsAvailable = !pinINT.read();
}

void SubDevice::pollEvents() {
    if (eventsAvailable) {
        uint8_t event[2];
        while (true) {
            auto valid = transact(READ_EVENTS, event, sizeof(event));
            if (!valid) {
                break;
            }

            if (event[0] == 0x00) {
                break;
            }

            uint8_t status;
            transact(ACK_EVENT, &status, 1);

            logger->info("[%i] Event: %02x.%02x", number, event[0], event[1]);
            if (event[0] == 0x11) {
                logger->info("[%i] Notifying?: %02x.%02x", number, event[0], event[1]);
                auto switchNumber = event[1];
                this->deviceSwitchEventSubject.notify(
                    {
                        .deviceId = 0x1234,
                        .switchNumber = switchNumber,
                        .state = tfw::hal::buttons::ButtonState::PRESSED
                    }
                );
            } else if (event[0] == 0x12) {
                logger->info("[%i] Notifying?: %02x.%02x", number, event[0], event[1]);
                auto switchNumber = event[1];
                this->deviceSwitchEventSubject.notify(
                    {
                        .deviceId = 0x1234,
                        .switchNumber = switchNumber,
                        .state = tfw::hal::buttons::ButtonState::UNPRESSED
                    }
                );
            }

            if (event[0] == 0x01) {
                scanScheduled = true;
            }
        }
    }

    eventsAvailable = !pinINT.read();
}

bool SubDevice::transact(const uint8_t command, uint8_t* response, const size_t length, const uint32_t pauseUs) {
    uint8_t crc = 0;

    spi.beginTransaction(spiSettings);
    pinCS.setLow();
    delayMicroseconds(5);

    spi.transfer(command);
    delayMicroseconds(pauseUs);
    for (size_t i = 0; i < length; i++) {
        response[i] = spi.transfer(0x00);
        delayMicroseconds(pauseUs);
    }
    crc = spi.transfer(0x00);

    pinCS.setHigh();
    spi.endTransaction();

    // Give the slave time to handle the end of the transaction
    delayMicroseconds(20);

    const uint8_t expected = crc8(command, response, length);

    /*
    if (crc != expected && logFailures) {
        Serial.printf("  command %02X -> ", command);
        for (size_t i = 0; i < length; i++) {
            Serial.printf("%02X ", response[i]);
        }
        Serial.printf("crc %02X (expected %02X)\n\r", crc, expected);
    }*/

    return crc == expected;
}
