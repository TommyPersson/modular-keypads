#include "MCP23x17.h"

#include <tfw/hal/time.h>

namespace tfw::ic {
    MCP23x17::MCP23x17(
        std::unique_ptr<tfw::utils::serialbus::SerialBus> bus,
        tfw::utils::gpio::OutputPin resetPin
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
        tfw::utils::time::delayMs(10);
        resetPin.setHigh();
        tfw::utils::time::delayMs(10);
    }

    std::unique_ptr<MCP23x17> spi(
        const tfw::utils::serialbus::SPIConfig& config,
        const tfw::utils::gpio::OutputPin resetPin
    ) {
        return std::make_unique<MCP23x17>(tfw::utils::serialbus::spi(config), resetPin);
    }
}
