#pragma once

#include <memory>
#include <SPI.h>

#include "../SerialBus.h"
#include "SPIConfig.h"

namespace utils::serialbus {
    class SPISerialBus final : public SerialBus {
    public:
        explicit SPISerialBus(const SPIConfig& config);
        ~SPISerialBus() override;

        uint8_t read8(uint8_t address, uint8_t reg) const override;
        uint8_t write8(uint8_t address, uint8_t reg, uint8_t data) const override;
        void begin() override;

    private:
        SPIConfig config;
        std::unique_ptr<SPIClass> spi;
        std::unique_ptr<SPISettings> spiSettings;
    };
}
