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

    const auto sws1Reg = this->addRegister("SWS1");
    const auto sws2Reg = this->addRegister("SWS2");

    this->switchMonitors.emplace_back(std::make_shared<SwitchMonitor>(1, BitReader::forRegister(*sws1Reg, 0)));
    this->switchMonitors.emplace_back(std::make_shared<SwitchMonitor>(2, BitReader::forRegister(*sws1Reg, 1)));
    this->switchMonitors.emplace_back(std::make_shared<SwitchMonitor>(3, BitReader::forRegister(*sws1Reg, 2)));
    this->switchMonitors.emplace_back(std::make_shared<SwitchMonitor>(4, BitReader::forRegister(*sws1Reg, 3)));
    this->switchMonitors.emplace_back(std::make_shared<SwitchMonitor>(5, BitReader::forRegister(*sws1Reg, 4)));
    this->switchMonitors.emplace_back(std::make_shared<SwitchMonitor>(6, BitReader::forRegister(*sws1Reg, 5)));
    this->switchMonitors.emplace_back(std::make_shared<SwitchMonitor>(7, BitReader::forRegister(*sws1Reg, 6)));
    this->switchMonitors.emplace_back(std::make_shared<SwitchMonitor>(8, BitReader::forRegister(*sws1Reg, 7)));

    this->switchMonitors.emplace_back(std::make_shared<SwitchMonitor>(9, BitReader::forRegister(*sws2Reg, 0)));
    this->switchMonitors.emplace_back(std::make_shared<SwitchMonitor>(10, BitReader::forRegister(*sws2Reg, 1)));
    this->switchMonitors.emplace_back(std::make_shared<SwitchMonitor>(11, BitReader::forRegister(*sws2Reg, 2)));
    this->switchMonitors.emplace_back(std::make_shared<SwitchMonitor>(12, BitReader::forRegister(*sws2Reg, 3)));
}

FirmwareModuleA::~FirmwareModuleA() = default;

int i = 0;
int j = 0;
Adafruit_NeoPixel pixels(12, 6, NEO_GRB + NEO_KHZ800);

void FirmwareModuleA::setup() {
    Firmware::setup();

    i = 0;
    j = 0;

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

    pixels.begin();
    pixels.setBrightness(50);


    for (auto& switchMonitor : this->switchMonitors) {
        switchMonitor->begin();
    }

    logger.info("FirmwareModuleA:started");
}

uint32_t sw9Pixel = 0;

void FirmwareModuleA::loop() {
    Firmware::loop();

    const auto& sws1Reg = this->registers->get("SWS1");
    const auto& sws2Reg = this->registers->get("SWS2");

    sws1Reg->write(mcp23x17->readPortB());
    sws2Reg->write(mcp23x17->readPortA());

    bool pixelsDirty = false;

    for (auto& switchMonitor : this->switchMonitors) {
        switchMonitor->update();
    }

    for (auto& switchMonitor : this->switchMonitors) {
        if (switchMonitor->getCurrentState() && !switchMonitor->getPreviousState()) {
            this->notifier.notify({.type = "switch.pressed", .args = { std::to_string(switchMonitor->getSwitchNumber()) }});
        }

        if (!switchMonitor->getCurrentState() && switchMonitor->getPreviousState()) {
            this->notifier.notify({.type = "switch.released", .args = { std::to_string(switchMonitor->getSwitchNumber()) }});
        }
    }

    pixels.setPixelColor(4, sw9Pixel);

    if (pixelsDirty) {
        pixels.show();
    }

    i++;
    if (i == 1000) {
        j++;
        i = 0;

        uint8_t gppua = mcp23x17->readRegister(MCP23x17_Registers::GPPUA);
        uint8_t gppub = mcp23x17->readRegister(MCP23x17_Registers::GPPUB);

        uint8_t iodira = mcp23x17->readRegister(MCP23x17_Registers::IODIRA);
        uint8_t iodirb = mcp23x17->readRegister(MCP23x17_Registers::IODIRB);
        /*
                logger.debug(
                    "%4i: SWS1=%2x, SWS2=%2x, DIRA=%2x, DIRB=%2x, GPPUA=%2x, GPPUB=%2x",
                    j,
                    sws1Reg->read(),
                    sws2Reg->read(),
                    iodira,
                    iodirb,
                    gppua,
                    gppub
                    );
        */
        pixels.setPixelColor(0, Adafruit_NeoPixel::Color(0xff, 0x00, 0x00));
        pixels.setPixelColor(1, Adafruit_NeoPixel::Color(0x00, 0xff, 0x00));
        pixels.setPixelColor(2, Adafruit_NeoPixel::Color(0x00, 0x00, 0xff));
        pixels.setPixelColor(3, Adafruit_NeoPixel::Color(0x00, 0xff, 0xff));
        pixels.show();
    }
}
