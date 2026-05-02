#include "LocalRegisterRefresherA.h"

#include "DeviceModuleA.h"

LocalRegisterRefresherA::LocalRegisterRefresherA(tfw::utils::registers::RegisterManager& registers) :
    RegisterRefresher(registers) {
    this->mcp23x17 = tfw::ic::spi(
        {
            .spiBus = FSPI,
            .pinSCK = tfw::hal::gpio::OutputPin(41),
            .pinMOSI = tfw::hal::gpio::OutputPin(40),
            .pinMISO = tfw::hal::gpio::InputPin::physical(39),
            .pinCS = tfw::hal::gpio::OutputPin(38),
        },
        tfw::hal::gpio::OutputPin(42)
    );
}

LocalRegisterRefresherA::~LocalRegisterRefresherA() = default;

void LocalRegisterRefresherA::setup() {
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

void LocalRegisterRefresherA::loop() {
    registers.write(devices::a::registers::IOA, mcp23x17->readPortA());
    registers.write(devices::a::registers::IOB, mcp23x17->readPortB());
}
