#pragma once

#ifdef ARDUINO

#include <memory>
#include <SPI.h>

#include "SPIConfig.h"

namespace tfw::hal::spi {
    class SPISerialBus final {
    public:
        explicit SPISerialBus(std::unique_ptr<SPIConfig> config);
        ~SPISerialBus();

        uint8_t read8(uint8_t address, uint8_t reg) const;
        uint8_t write8(uint8_t address, uint8_t reg, uint8_t data) const;
        void begin();

    private:
        std::unique_ptr<SPIConfig> config;
        std::unique_ptr<SPIClass> spi;
        std::unique_ptr<SPISettings> spiSettings;
    };
}

#endif // ARDUINO
