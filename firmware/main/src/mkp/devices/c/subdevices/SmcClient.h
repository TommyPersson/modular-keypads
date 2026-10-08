#pragma once

#include <span>
#include <SPI.h>

#include "tfw/hal/gpio/OutputPin.h"

namespace mkp::devices::c {
    // Must match the SMC firmware, see firmware/smc/src/hal/spi/SpiSlave.h
    namespace smc {
        namespace commands {
            inline constexpr uint8_t READ_DEVICE_ID = 0x01;
            inline constexpr uint8_t READ_DEVICE_TYPE = 0x02;
            inline constexpr uint8_t READ_EVENTS = 0x03;
            inline constexpr uint8_t ACK_EVENT = 0x04;
            inline constexpr uint8_t FLASH_IDENTIFICATION_LIGHTS = 0x05;
            inline constexpr uint8_t FLASH_BUTTON_IDENTIFICATION_LIGHT = 0x06;
            inline constexpr uint8_t READ_DEVICE_NAME = 0x07;
            inline constexpr uint8_t WRITE_DEVICE_NAME = 0x08;
        }

        // Including the terminating null character, see firmware/smc/src/hal/identity/DeviceName.h
        inline constexpr uint8_t DEVICE_NAME_LENGTH = 16;

        // Generic responses, for commands that have no more specific response
        namespace responses {
            inline constexpr uint8_t ACCEPTED = 0x01;
        }
    }

    /**
     * SPI master side of the SMC protocol: a command byte, its argument bytes, the response bytes and a CRC-8
     * over all of them, with a pause after every byte for the SMC to load the next one.
     */
    class SmcClient {
    public:
        SmcClient(SPIClass& spi, tfw::hal::gpio::OutputPin& pinCS);

        void setup();

        /**
         * Retries up to MAX_TRANSACT_ATTEMPTS times when the CRC doesn't match, e.g. when the SMC had interrupts
         * disabled during the transaction. Only use with commands that are safe to repeat.
         */
        bool transact(uint8_t command, uint8_t* response, size_t length, uint32_t pauseUs = DEFAULT_PAUSE_US);
        bool transact(
            uint8_t command,
            std::span<const uint8_t> arguments,
            uint8_t* response,
            size_t length,
            uint32_t pauseUs = DEFAULT_PAUSE_US
        );

    private:
        static constexpr uint32_t DEFAULT_PAUSE_US = 50;
        static constexpr uint8_t MAX_TRANSACT_ATTEMPTS = 3;

        SPIClass& spi;
        tfw::hal::gpio::OutputPin& pinCS;

        bool transactOnce(
            uint8_t command,
            std::span<const uint8_t> arguments,
            uint8_t* response,
            size_t length,
            uint32_t pauseUs
        );
    };
}
