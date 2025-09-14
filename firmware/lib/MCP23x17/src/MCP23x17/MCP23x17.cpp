#include "MCP23x17.h"

MCP23x17::MCP23x17(std::unique_ptr<SerialBus> bus) : bus(std::move(bus)) {
}
MCP23x17::~MCP23x17() {
}

uint8_t MCP23x17::readPortA() const {
    return bus->read8(0x00, 0x12);
}

uint8_t MCP23x17::readPortB() const {
    return bus->read8(0x00, 0x13);
}

void MCP23x17::begin() const {
    bus->begin();

    // TODO Move to configuration function
    bus->write8(0x00, 0x00, 0xff);
    bus->write8(0x00, 0x01, 0xff);

    bus->write8(0x00, 0x0c, 0xff);
    bus->write8(0x00, 0x0d, 0xff);
}

std::unique_ptr<MCP23x17> MCP23x17::SPI(const SPIConfig& config) {
    return std::make_unique<MCP23x17>(SerialBus::SPI(config));
}
