#include "MCP23x17.h"

#include <esp32-hal.h>

namespace chips::mcp23x17 {
    MCP23x17::MCP23x17(
        std::unique_ptr<utils::serialbus::SerialBus> bus,
        utils::pins::OutputPin resetPin
    ) : bus(std::move(bus)),
        resetPin(resetPin) {
    }

    MCP23x17::~MCP23x17() {
    }

    uint8_t MCP23x17::readPortA() const {
        return bus->read8(0x00, registers::GPIOA);
    }

    uint8_t MCP23x17::readPortB() const {
        return bus->read8(0x00, registers::GPIOB);
    }

    uint8_t MCP23x17::readRegister(uint8_t reg) const {
        return bus->read8(0x00, reg);
    }

    uint8_t MCP23x17::writeRegister(uint8_t reg, uint8_t value) {
        return bus->write8(0x00, reg, value);
    }

    void MCP23x17::begin() {
        bus->begin();
        resetPin.init();

        resetPin.setLow();
        delay(10);
        resetPin.setHigh();
        delay(10);
    }

    std::unique_ptr<MCP23x17> spi(
        const utils::serialbus::SPIConfig& config,
        const utils::pins::OutputPin resetPin
    ) {
        return std::make_unique<MCP23x17>(utils::serialbus::spi(config), resetPin);
    }
}
