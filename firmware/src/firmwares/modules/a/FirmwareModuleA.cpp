#include "FirmwareModuleA.h"

#include "firmwares/common/i2c/EndpointStructs.h"

#include <Adafruit_NeoPixel.h>
#include <SPI.h>

FirmwareModuleA::FirmwareModuleA(
    DeviceConfigurationManager& deviceConfigurationManager,
    SerialPort& serialPort,
    Notifier& notifier,
    Logger& logger,
    TwoWire& i2c
    ) :
    Firmware(deviceConfigurationManager, serialPort, notifier, logger, i2c) {

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

    switchStateChangeNotifier = std::make_unique<SwitchStateChangeNotifier>(notifier);

    i2cSlavePort = std::make_unique<i2c::SlavePort>(i2c, logger);

    const auto& ioaReg = this->addRegister("IOA");
    const auto& iobReg = this->addRegister("IOB");

    this->attachSwitch(1, BitReader::forRegister(*iobReg, 0), 0);
    this->attachSwitch(2, BitReader::forRegister(*iobReg, 1), 11);
    this->attachSwitch(3, BitReader::forRegister(*iobReg, 2), 1);
    this->attachSwitch(4, BitReader::forRegister(*iobReg, 3), 10);
    this->attachSwitch(5, BitReader::forRegister(*iobReg, 4), 2);
    this->attachSwitch(6, BitReader::forRegister(*iobReg, 5), 9);
    this->attachSwitch(7, BitReader::forRegister(*iobReg, 6), 3);
    this->attachSwitch(8, BitReader::forRegister(*iobReg, 7), 8);

    this->attachSwitch(9, BitReader::forRegister(*ioaReg, 0), 4);
    this->attachSwitch(10, BitReader::forRegister(*ioaReg, 1), 7);
    this->attachSwitch(11, BitReader::forRegister(*ioaReg, 2), 5);
    this->attachSwitch(12, BitReader::forRegister(*ioaReg, 3), 6);
}

FirmwareModuleA::~FirmwareModuleA() {
    for (const auto& switchMonitor : this->switchMonitors) {
        switchMonitor->onSwitchStateChanged().removeObserver(switchStateChangeNotifier.get());
    }
}

void FirmwareModuleA::setup() {
    Firmware::setup();

    auto deviceAddress = deviceConfigurationManager.getDeviceAddress();
    if (deviceAddress > 0) {
        const auto deviceId = deviceConfigurationManager.getDeviceId();
        i2c::structs::DeviceInformation deviceInformationStruct;
        std::memcpy(&deviceInformationStruct.deviceId, deviceId.data(), sizeof(deviceInformationStruct.deviceId));
        deviceInformationStruct.deviceType = deviceConfigurationManager.getDeviceType();

        i2cSlavePort->updateEndpoint(i2c::Endpoint::DeviceInformation, &deviceInformationStruct, sizeof(deviceInformationStruct));
        i2cSlavePort->begin(deviceAddress, 1, 0);
    }

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
        switchMonitor->onSwitchStateChanged().addObserver(switchStateChangeNotifier.get());
    }

    logger.info("FirmwareModuleA:started");
}

void FirmwareModuleA::loop() {
    Firmware::loop();

    this->registers->get("IOB")->write(mcp23x17->readPortB());
    this->registers->get("IOA")->write(mcp23x17->readPortA());

    for (const auto& switchMonitor : this->switchMonitors) {
        switchMonitor->update();
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
