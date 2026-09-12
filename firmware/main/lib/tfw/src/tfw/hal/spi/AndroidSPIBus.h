#pragma once

#ifdef ARDUINO

#include <memory>
#include <SPI.h>

#include "SPIConfig.h"
#include "SPIBus.h"

namespace tfw::hal::spi {
    class AndroidSPIBus final : public SPIBus {
    public:
        explicit AndroidSPIBus(std::unique_ptr<SPIConfig> config);
        ~AndroidSPIBus();

        uint8_t read8(uint8_t address, uint8_t reg) const override;
        uint8_t write8(uint8_t address, uint8_t reg, uint8_t data) override;
        void begin() override;

    private:
        std::unique_ptr<SPIConfig> config;
        std::unique_ptr<SPIClass> spi;
        std::unique_ptr<SPISettings> spiSettings;
    };
}

#endif // ARDUINO
