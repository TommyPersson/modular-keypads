#pragma once

#include <memory>

#include "spi/SPIConfig.h"

class SerialBus {
public:
    virtual ~SerialBus() {}

    virtual uint8_t read8(uint8_t address, uint8_t reg) const = 0;
    virtual uint8_t write8(uint8_t address, uint8_t reg, uint8_t data) const = 0;
    virtual void begin() = 0;

    static std::unique_ptr<SerialBus> SPI(const SPIConfig& config);
};

