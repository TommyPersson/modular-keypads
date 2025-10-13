#include "FirmwareModuleA.h"

#include <Adafruit_NeoPixel.h>
#include <SPI.h>

FirmwareModuleA::FirmwareModuleA(
    DeviceConfigurationManager& deviceConfigurationManager,
    SerialPort& serialPort,
    Notifier& notifier,
    Logger& logger
    ) :
    Firmware(deviceConfigurationManager, serialPort, notifier, logger) {
    this->mcp23x17 = MCP23x17::SPI(
        {
            .spiBus = FSPI,
            .pinSCK = OutputPin(4),
            .pinMOSI = OutputPin(3),
            .pinMISO = InputPin::physical(2),
            .pinCS = OutputPin(5),
        }
        );

    this->indicatorLeds = IndicatorLedManager::NeoPixel(12, 6);

    const auto& sws1Reg = this->addRegister("SWS1");
    const auto& sws2Reg = this->addRegister("SWS2");

    this->attachSwitch(1, BitReader::forRegister(*sws1Reg, 0), 0);
    this->attachSwitch(2, BitReader::forRegister(*sws1Reg, 1), 11);
    this->attachSwitch(3, BitReader::forRegister(*sws1Reg, 2), 1);
    this->attachSwitch(4, BitReader::forRegister(*sws1Reg, 3), 10);
    this->attachSwitch(5, BitReader::forRegister(*sws1Reg, 4), 2);
    this->attachSwitch(6, BitReader::forRegister(*sws1Reg, 5), 9);
    this->attachSwitch(7, BitReader::forRegister(*sws1Reg, 6), 3);
    this->attachSwitch(8, BitReader::forRegister(*sws1Reg, 7), 8);

    this->attachSwitch(9, BitReader::forRegister(*sws2Reg, 0), 4);
    this->attachSwitch(10, BitReader::forRegister(*sws2Reg, 1), 7);
    this->attachSwitch(11, BitReader::forRegister(*sws2Reg, 2), 5);
    this->attachSwitch(12, BitReader::forRegister(*sws2Reg, 3), 6);
}

FirmwareModuleA::~FirmwareModuleA() = default;

void FirmwareModuleA::setup() {
    Firmware::setup();

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

    indicatorLeds->begin();

    for (auto& switchMonitor : this->switchMonitors) {
        switchMonitor->begin();
    }

    logger.info("FirmwareModuleA:started");
}

void FirmwareModuleA::loop() {
    Firmware::loop();

    this->registers->get("SWS1")->write(mcp23x17->readPortB());
    this->registers->get("SWS2")->write(mcp23x17->readPortA());

    for (auto& switchMonitor : this->switchMonitors) {
        switchMonitor->update();
    }

    for (auto& switchMonitor : this->switchMonitors) {
        if (switchMonitor->getCurrentState() == SwitchState::PRESSED &&
            switchMonitor->getPreviousState() == SwitchState::UNPRESSED) {
            this->notifier.notify(
                {
                    .type = "switch.pressed",
                    .args = {std::to_string(switchMonitor->getSwitchNumber())}
                }
                );
        }

        if (switchMonitor->getCurrentState() == SwitchState::UNPRESSED &&
            switchMonitor->getPreviousState() == SwitchState::PRESSED) {
            this->notifier.notify(
                {
                    .type = "switch.released",
                    .args = {std::to_string(switchMonitor->getSwitchNumber())}
                }
                );
        }
    }

    indicatorLeds->update();
}

void FirmwareModuleA::attachSwitch(
    const uint8_t number,
    const std::shared_ptr<BitReader>& bitReader,
    const uint8_t ledIndex
    ) {
    const auto switchMonitor = this->switchMonitors.emplace_back(std::make_shared<SwitchMonitor>(number, bitReader));
    indicatorLeds->connectToSwitch(ledIndex, *switchMonitor);
}
