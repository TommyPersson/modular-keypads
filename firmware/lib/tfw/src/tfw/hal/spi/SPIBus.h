#pragma once

#include <cstdint>

namespace tfw::hal::spi {
    /**
     * Abstract interface for SPI bus communication.
     * Defines the contract for reading and writing 8-bit values over SPI.
     */
    class SPIBus {
    public:
        virtual ~SPIBus() = default;

        /**
         * Read an 8-bit value from a register.
         * @param address Device address
         * @param reg Register address
         * @return The read value
         */
        virtual uint8_t read8(uint8_t address, uint8_t reg) const = 0;

        /**
         * Write an 8-bit value to a register.
         * @param address Device address
         * @param reg Register address
         * @param data The value to write
         * @return The read/echo value
         */
        virtual uint8_t write8(uint8_t address, uint8_t reg, uint8_t data) = 0;

        /**
         * Initialize the SPI bus.
         */
        virtual void begin() = 0;
    };
}
