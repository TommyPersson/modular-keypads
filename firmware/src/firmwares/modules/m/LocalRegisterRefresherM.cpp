#include "LocalRegisterRefresherM.h"

#include "DeviceModuleM.h"

LocalRegisterRefresherM::LocalRegisterRefresherM(utils::registers::RegisterManager& registers) :
    RegisterRefresher(registers) {
    mcpResetPin = std::make_unique<utils::pins::OutputPin>(8);

    mcp23x17 = chips::mcp23x17::spi(
        {
            .spiBus = FSPI,
            .pinSCK = utils::pins::OutputPin(5),
            .pinMOSI = utils::pins::OutputPin(4),
            .pinMISO = utils::pins::InputPin::physical(3),
            .pinCS = utils::pins::OutputPin(6),
        }
    );
}

LocalRegisterRefresherM::~LocalRegisterRefresherM() = default;

void LocalRegisterRefresherM::setup() {
    mcpResetPin->init();
    mcpResetPin->setHigh();

    mcp23x17->begin();

    // Set all pins as input
    mcp23x17->writeRegister(chips::mcp23x17::registers::IODIRA, 0xff);
    mcp23x17->writeRegister(chips::mcp23x17::registers::IODIRB, 0xff);

    // Enable pull ups
    mcp23x17->writeRegister(chips::mcp23x17::registers::GPPUA, 0xff);
    mcp23x17->writeRegister(chips::mcp23x17::registers::GPPUB, 0xff);

    // Invert the read values
    mcp23x17->writeRegister(chips::mcp23x17::registers::IOPOLA, 0xff);
    mcp23x17->writeRegister(chips::mcp23x17::registers::IOPOLB, 0xff);
}

void LocalRegisterRefresherM::loop() {
    registers.write(devices::m::registers::IOA, mcp23x17->readPortA());
    registers.write(devices::m::registers::IOB, mcp23x17->readPortB());
}
