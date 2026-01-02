#include "SPISerialBus.h"

#include <Arduino.h>

namespace utils::serialbus {
    SPISerialBus::SPISerialBus(const SPIConfig& config) :
        config(config) {
        spi = std::make_unique<SPIClass>(config.spiBus);
        spiSettings = std::make_unique<SPISettings>(1000000, MSBFIRST, SPI_MODE0);
    }

    SPISerialBus::~SPISerialBus() {
    }

    uint8_t SPISerialBus::read8(uint8_t address, uint8_t reg) const {
        // TODO use address

        const int8_t readOperation = 0b01000001;

        config.pinCS.setLow();
        spi->beginTransaction(*spiSettings);
        spi->transfer(readOperation);
        spi->transfer(reg);
        const uint8_t response = spi->transfer(0xFF);
        spi->endTransaction();
        config.pinCS.setHigh();

        return response;
    }

    uint8_t SPISerialBus::write8(uint8_t address, uint8_t reg, uint8_t data) const {
        // TODO use address

        const int8_t writeOperation = 0b01000000;

        config.pinCS.setLow();
        spi->beginTransaction(*spiSettings);
        spi->transfer(writeOperation);
        spi->transfer(reg);
        const uint8_t response = spi->transfer(data);
        spi->endTransaction();
        config.pinCS.setHigh();

        return response;
    }

    void SPISerialBus::begin() {
        config.pinSCK.init();
        config.pinMOSI.init();
        config.pinMISO->init();
        config.pinCS.init();
        config.pinCS.setHigh();

        spi->begin(config.pinSCK.pinNumber, config.pinMISO->pinNumber, config.pinMOSI.pinNumber, config.pinCS.pinNumber);
    }
}