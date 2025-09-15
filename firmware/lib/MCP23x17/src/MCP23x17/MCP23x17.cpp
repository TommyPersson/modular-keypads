#include "MCP23x17.h"

MCP23x17::MCP23x17(std::unique_ptr<SerialBus> bus) : bus(std::move(bus)) {
}
MCP23x17::~MCP23x17() {
}

uint8_t MCP23x17::readPortA() const {
    return bus->read8(0x00, MCP23x17_Registers::GPIOA);
}

uint8_t MCP23x17::readPortB() const {
    return bus->read8(0x00, MCP23x17_Registers::GPIOB);
}

uint8_t MCP23x17::readRegister(uint8_t reg) const {
    return bus->read8(0x00, reg);
}

uint8_t MCP23x17::writeRegister(uint8_t reg, uint8_t value) {
    return bus->write8(0x00, reg, value);
}

void MCP23x17::begin() {
    bus->begin();
}

std::unique_ptr<MCP23x17> MCP23x17::SPI(const SPIConfig& config) {
    return std::make_unique<MCP23x17>(SerialBus::SPI(config));
}
