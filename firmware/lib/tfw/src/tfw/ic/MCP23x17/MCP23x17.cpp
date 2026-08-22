#ifdef ARDUINO

#include "MCP23x17.h"

#include <tfw/hal/time.h>

namespace tfw::ic {
    MCP23x17::MCP23x17(
        std::unique_ptr<tfw::hal::spi::SPISerialBus> bus,
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
        tfw::hal::time::delayMs(10);
        resetPin->setHigh();
        tfw::hal::time::delayMs(10);
    }

    std::unique_ptr<MCP23x17> spi(
        tfw::hal::spi::SPIConfig config,
        std::unique_ptr<tfw::hal::gpio::OutputPin> resetPin
    ) {
        auto configPtr = std::make_unique<tfw::hal::spi::SPIConfig>(std::move(config));
        return std::make_unique<MCP23x17>(std::make_unique<tfw::hal::spi::SPISerialBus>(std::move(configPtr)), std::move(resetPin));
    }
}

#endif // ARDUINO
