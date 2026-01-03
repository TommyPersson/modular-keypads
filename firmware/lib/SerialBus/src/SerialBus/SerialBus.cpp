#include "SerialBus.h"

#include "spi/SPISerialBus.h"

// TODO degeneralize, I don't need/want/can treat SPI and I2C the same way anyway
namespace utils::serialbus {
    std::unique_ptr<SerialBus> spi(const SPIConfig& config) {
        return std::make_unique<SPISerialBus>(config);
    }
}
