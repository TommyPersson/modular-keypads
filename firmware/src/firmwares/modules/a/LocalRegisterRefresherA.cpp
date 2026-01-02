#include "LocalRegisterRefresherA.h"

#include "DeviceModuleA.h"

LocalRegisterRefresherA::LocalRegisterRefresherA(utils::registers::RegisterManager& registers) :
    RegisterRefresher(registers) {
    this->mcp23x17 = chips::mcp23x17::spi(
        {
            .spiBus = FSPI,
            .pinSCK = utils::pins::OutputPin(4),
            .pinMOSI = utils::pins::OutputPin(3),
            .pinMISO = utils::pins::InputPin::physical(2),
            .pinCS = utils::pins::OutputPin(5),
        }
    );
}

LocalRegisterRefresherA::~LocalRegisterRefresherA() = default;

void LocalRegisterRefresherA::begin() {
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

void LocalRegisterRefresherA::loop() {
    registers.write(devices::a::registers::IOA, mcp23x17->readPortA());
    registers.write(devices::a::registers::IOB, mcp23x17->readPortB());
}
