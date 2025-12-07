#include "LocalRegisterRefresherM.h"

#include <SPI.h>

LocalRegisterRefresherM::LocalRegisterRefresherM(RegisterManager& registers) :
    RegisterRefresher(registers) {

    mcpResetPin = std::make_unique<OutputPin>(8);

    mcp23x17 = MCP23x17::SPI(
        {
            .spiBus = FSPI,
            .pinSCK = OutputPin(5),
            .pinMOSI = OutputPin(4),
            .pinMISO = InputPin::physical(3),
            .pinCS = OutputPin(6),
        }
        );
}

LocalRegisterRefresherM::~LocalRegisterRefresherM() = default;

void LocalRegisterRefresherM::begin() {
    mcpResetPin->init();
    mcpResetPin->setHigh();

    mcp23x17->begin();

    // Set all pins as input
    mcp23x17->writeRegister(MCP23x17_Registers::IODIRA, 0xff);
    mcp23x17->writeRegister(MCP23x17_Registers::IODIRB, 0xff);

    // Enable pull ups
    mcp23x17->writeRegister(MCP23x17_Registers::GPPUA, 0xff);
    mcp23x17->writeRegister(MCP23x17_Registers::GPPUB, 0xff);

    // Invert the read values
    mcp23x17->writeRegister(MCP23x17_Registers::IOPOLA, 0xff);
    mcp23x17->writeRegister(MCP23x17_Registers::IOPOLB, 0xff);
}

void LocalRegisterRefresherM::loop() {
    registers.get("IOB")->write(mcp23x17->readPortB());
    registers.get("IOA")->write(mcp23x17->readPortA());
}
