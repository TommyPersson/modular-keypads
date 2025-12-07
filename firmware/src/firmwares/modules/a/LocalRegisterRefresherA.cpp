#include "LocalRegisterRefresherA.h"

#include <SPI.h>

LocalRegisterRefresherA::LocalRegisterRefresherA(RegisterManager& registers) :
    RegisterRefresher(registers) {

    this->mcp23x17 = MCP23x17::SPI(
        {
            .spiBus = FSPI,
            .pinSCK = OutputPin(4),
            .pinMOSI = OutputPin(3),
            .pinMISO = InputPin::physical(2),
            .pinCS = OutputPin(5),
        }
        );
}

LocalRegisterRefresherA::~LocalRegisterRefresherA() = default;

void LocalRegisterRefresherA::begin() {
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

void LocalRegisterRefresherA::loop() {
    registers.get("IOB")->write(mcp23x17->readPortB());
    registers.get("IOA")->write(mcp23x17->readPortA());
}
