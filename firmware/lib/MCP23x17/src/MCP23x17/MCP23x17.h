#pragma once

#include <memory>

#include "SerialBus/spi/SPIConfig.h"
#include "SerialBus/SerialBus.h"

namespace chips::mcp23x17 {
    class MCP23x17 {
    public:
        explicit MCP23x17(std::unique_ptr<utils::serialbus::SerialBus> bus);
        ~MCP23x17();

        uint8_t readPortA() const;
        uint8_t readPortB() const;
        uint8_t readRegister(uint8_t reg) const;
        uint8_t writeRegister(uint8_t reg, uint8_t value);

        void begin();

    private:
        const std::unique_ptr<utils::serialbus::SerialBus> bus;
    };

    std::unique_ptr<MCP23x17> spi(const utils::serialbus::SPIConfig& config);

    namespace registers {
        constexpr uint8_t IODIRA = 0x00;
        constexpr uint8_t IODIRB = 0x01;
        constexpr uint8_t IOPOLA = 0x02;
        constexpr uint8_t IOPOLB = 0x03;
        constexpr uint8_t GPPUA = 0x0c;
        constexpr uint8_t GPPUB = 0x0d;
        constexpr uint8_t GPIOA = 0x12;
        constexpr uint8_t GPIOB = 0x13;
    }
}
