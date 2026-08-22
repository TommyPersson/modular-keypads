#include "RegisterRefresherA.h"

#include "LocalDeviceA.h"
#include <tfw/hal/spi/SPIConfig.h>
#include <tfw/hal/spi/AndroidSPIBus.h>

using namespace mkp::devices::a;

RegisterRefresherA::RegisterRefresherA(tfw::utils::registers::RegisterManager& registers, tfw::hal::time::Clock& clock) :
    RegisterRefresher(registers) {
    // Create SPI configuration
    auto spiConfig = std::make_unique<tfw::hal::spi::SPIConfig>(tfw::hal::spi::SPIConfig{
        .spiBus = FSPI,
        .pinSCK = tfw::hal::gpio::OutputPin::physical(41),
        .pinMOSI = tfw::hal::gpio::OutputPin::physical(40),
        .pinMISO = tfw::hal::gpio::InputPin::physical(39),
        .pinCS = tfw::hal::gpio::OutputPin::physical(38),
    });

    // Create SPI serial bus
    auto serialBus = std::make_unique<tfw::hal::spi::AndroidSPIBus>(std::move(spiConfig));

    // Create MCP23x17 with serial bus, reset pin, and clock
    this->mcp23x17 = std::make_unique<tfw::ic::MCP23x17>(
        std::move(serialBus),
        tfw::hal::gpio::OutputPin::physical(42),
        clock
    );
}

RegisterRefresherA::~RegisterRefresherA() = default;

void RegisterRefresherA::setup() {
    mcp23x17->begin();

    // Set all pins as input
    mcp23x17->writeRegister(tfw::ic::registers::IODIRA, 0xff);
    mcp23x17->writeRegister(tfw::ic::registers::IODIRB, 0xff);

    // Enable pull ups
    mcp23x17->writeRegister(tfw::ic::registers::GPPUA, 0xff);
    mcp23x17->writeRegister(tfw::ic::registers::GPPUB, 0xff);

    // Invert the read values
    mcp23x17->writeRegister(tfw::ic::registers::IOPOLA, 0xff);
    mcp23x17->writeRegister(tfw::ic::registers::IOPOLB, 0xff);
}

void RegisterRefresherA::loop() {
    registers.write(devices::a::registers::IOA, mcp23x17->readPortA());
    registers.write(devices::a::registers::IOB, mcp23x17->readPortB());
}
