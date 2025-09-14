#pragma once

#include <memory>

#include "spi/SPIConfig.h"
#include "SerialBus.h"

class MCP23x17 {
public:
    explicit MCP23x17(std::unique_ptr<SerialBus> bus);
    ~MCP23x17();

    uint8_t readPortA() const;
    uint8_t readPortB() const;
    void begin() const;

    static std::unique_ptr<MCP23x17> SPI(const SPIConfig& config);

private:
    const std::unique_ptr<SerialBus> bus;
};
