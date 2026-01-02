#include "SerialBus.h"

#include "spi/SPISerialBus.h"

namespace utils::serialbus {
    std::unique_ptr<SerialBus> spi(const SPIConfig& config) {
        return std::make_unique<SPISerialBus>(config);
    }
}
