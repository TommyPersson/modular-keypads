#include "SerialBus.h"

#include "spi/SPISerialBus.h"

std::unique_ptr<SerialBus> SerialBus::SPI(const SPIConfig& config) {
    return std::make_unique<SPISerialBus>(config);
}
