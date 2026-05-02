#include "LocalRegisterRefresherM.h"

#include "DeviceModuleM.h"

LocalRegisterRefresherM::LocalRegisterRefresherM(tfw::utils::registers::RegisterManager& registers) :
    RegisterRefresher(registers) {

    mcp23x17 = tfw::ic::spi(
        {
            .spiBus = FSPI,
            .pinSCK = tfw::utils::gpio::OutputPin(5),
            .pinMOSI = tfw::utils::gpio::OutputPin(4),
            .pinMISO = tfw::utils::gpio::InputPin::physical(3),
            .pinCS = tfw::utils::gpio::OutputPin(6),
        },
        tfw::utils::gpio::OutputPin(42)
    );
}

LocalRegisterRefresherM::~LocalRegisterRefresherM() = default;

void LocalRegisterRefresherM::setup() {
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

void LocalRegisterRefresherM::loop() {
    registers.write(devices::m::registers::IOA, mcp23x17->readPortA());
    registers.write(devices::m::registers::IOB, mcp23x17->readPortB());
}
