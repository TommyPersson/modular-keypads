#include "MasterFirmware.h"

#include <Adafruit_NeoPixel.h>
#include <SPI.h>

MasterFirmware::MasterFirmware(
    DeviceConfigurationManager& deviceConfigurationManager,
    SerialPort& serialPort,
    Notifier& notifier,
    Logger& logger
    ) :
    Firmware(deviceConfigurationManager, serialPort, notifier, logger) {

    mcp23x17 = MCP23x17::SPI(
        {
            .spiBus = FSPI,
            .pinSCK = OutputPin(5),
            .pinMOSI = OutputPin(4),
            .pinMISO = InputPin::physical(3),
            .pinCS = OutputPin(6),
        }
        );

    indicatorLeds = IndicatorLedManager::NeoPixel(5, 7);

    switchStateChangeNotifier = std::make_unique<SwitchStateChangeNotifier>(notifier);
    encoderRotationNotifier = std::make_unique<EncoderRotationNotifier>(notifier);

    mcpResetPin = std::make_unique<OutputPin>(8);

    const auto& ioaReg = this->addRegister("IOA");
    const auto& iobReg = this->addRegister("IOB");

    this->attachSwitch(1, BitReader::forRegister(*iobReg, 0), 0);
    this->attachSwitch(2, BitReader::forRegister(*iobReg, 1), 1);
    this->attachSwitch(3, BitReader::forRegister(*iobReg, 2), 2);
    this->attachSwitch(4, BitReader::forRegister(*iobReg, 3), 3);

    this->attachSwitch(5, BitReader::forRegister(*iobReg, 4), -1);

    this->attachRotationalEncoder(
        1,
        BitReader::forRegister(*ioaReg, 0, BitReaderMode::Inverted),
        BitReader::forRegister(*ioaReg, 1, BitReaderMode::Inverted)
        );

}

MasterFirmware::~MasterFirmware() {
    for (const auto& switchMonitor : this->switchMonitors) {
        switchMonitor->onSwitchStateChanged().removeObserver(switchStateChangeNotifier.get());
    }
}

void MasterFirmware::setup() {
    Firmware::setup();

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

    indicatorLeds->begin();

    for (const auto& switchMonitor : this->switchMonitors) {
        switchMonitor->begin();
        switchMonitor->onSwitchStateChanged().addObserver(switchStateChangeNotifier.get());
    }

    for (const auto& encoderMonitor : this->rotationalEncoderMonitors) {
        encoderMonitor->begin();
        encoderMonitor->onEncoderRotated().addObserver(encoderRotationNotifier.get());
    }

    logger.info("MasterFirmware:started");
}

void MasterFirmware::loop() {
    Firmware::loop();

    this->registers->get("IOA")->write(mcp23x17->readPortA());
    this->registers->get("IOB")->write(mcp23x17->readPortB());

    for (const auto& switchMonitor : this->switchMonitors) {
        switchMonitor->update();
    }

    for (const auto& encoderMonitor : this->rotationalEncoderMonitors) {
        encoderMonitor->update();
    }

    indicatorLeds->update();
}

void MasterFirmware::attachSwitch(
    const uint8_t number,
    const std::shared_ptr<BitReader>& bitReader,
    const int ledIndex
    ) {
    const auto switchMonitor = this->switchMonitors.emplace_back(std::make_shared<SwitchMonitor>(number, bitReader));
    if (ledIndex >= 0) {
        indicatorLeds->connectToSwitch(ledIndex, *switchMonitor);
    }
}

void MasterFirmware::attachRotationalEncoder(
    const uint8_t number,
    const std::shared_ptr<BitReader>& aBitReader,
    const std::shared_ptr<BitReader>& bBitReader
    ) {
    this->rotationalEncoderMonitors.emplace_back(
        std::make_shared<RotationalEncoderMonitor>(number, aBitReader, bBitReader)
        );

}
