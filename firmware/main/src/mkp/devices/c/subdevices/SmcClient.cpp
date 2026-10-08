#include "SmcClient.h"

#include <Arduino.h>

using namespace mkp::devices::c;
using namespace tfw::hal::gpio;

namespace {
    const SPISettings spiSettings(1000000, MSBFIRST, SPI_MODE0);

    // CRC-8/AUTOSAR: poly 0x2F, init 0xFF, no reflection, xorout 0xFF
    uint8_t crc8(
        const uint8_t command,
        const std::span<const uint8_t> arguments,
        const uint8_t* responseData,
        const size_t responseLength
    ) {
        uint8_t crc = 0xFF;
        auto feed = [&crc](const uint8_t byte) {
            crc ^= byte;
            for (int bit = 0; bit < 8; bit++) {
                crc = crc & 0x80 ? (uint8_t)((crc << 1) ^ 0x2F) : (uint8_t)(crc << 1);
            }
        };

        feed(command);
        for (const auto argument : arguments) {
            feed(argument);
        }
        for (size_t i = 0; i < responseLength; i++) {
            feed(responseData[i]);
        }
        return crc ^ 0xFF;
    }
}

SmcClient::SmcClient(SPIClass& spi, OutputPin& pinCS)
    : spi(spi),
      pinCS(pinCS) {
}

void SmcClient::setup() {
    pinCS.init();
    pinCS.setHigh();
}

bool SmcClient::transact(const uint8_t command, uint8_t* response, const size_t length, const uint32_t pauseUs) {
    return transact(command, {}, response, length, pauseUs);
}

bool SmcClient::transact(
    const uint8_t command,
    const std::span<const uint8_t> arguments,
    uint8_t* response,
    const size_t length,
    const uint32_t pauseUs
) {
    for (uint8_t attempt = 0; attempt < MAX_TRANSACT_ATTEMPTS; attempt++) {
        if (transactOnce(command, arguments, response, length, pauseUs)) {
            return true;
        }
    }

    return false;
}

bool SmcClient::transactOnce(
    const uint8_t command,
    const std::span<const uint8_t> arguments,
    uint8_t* response,
    const size_t length,
    const uint32_t pauseUs
) {
    uint8_t crc = 0;

    spi.beginTransaction(spiSettings);
    pinCS.setLow();
    delayMicroseconds(5);

    spi.transfer(command);
    delayMicroseconds(pauseUs);
    for (const auto argument : arguments) {
        spi.transfer(argument);
        delayMicroseconds(pauseUs);
    }
    for (size_t i = 0; i < length; i++) {
        response[i] = spi.transfer(0x00);
        delayMicroseconds(pauseUs);
    }
    crc = spi.transfer(0x00);

    pinCS.setHigh();
    spi.endTransaction();

    // Give the slave time to handle the end of the transaction
    delayMicroseconds(20);

    // Also covers the arguments, so a mismatch includes the slave having received the wrong arguments
    const uint8_t expected = crc8(command, arguments, response, length);

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
