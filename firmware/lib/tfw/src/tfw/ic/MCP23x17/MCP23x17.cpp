#include "MCP23x17.h"

namespace tfw::ic {
    MCP23x17::MCP23x17(
        std::unique_ptr<tfw::hal::spi::SPIBus> bus,
        std::unique_ptr<tfw::hal::gpio::OutputPin> resetPin
    ) : bus(std::move(bus)),
        resetPin(std::move(resetPin)) {
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
        resetPin->init();

        resetPin->setLow();
        resetPin->setHigh();
    }
}
