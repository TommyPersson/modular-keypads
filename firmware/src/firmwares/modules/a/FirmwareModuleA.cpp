#include "FirmwareModuleA.h"

#include <Adafruit_NeoPixel.h>
#include <SPI.h>

FirmwareModuleA::FirmwareModuleA(
    DeviceConfigurationManager& deviceConfigurationManager,
    SerialPort& serialPort,
    Logger& logger
    ) :
    Firmware(deviceConfigurationManager, serialPort, logger) {

    this->mcp23x17 = MCP23x17::SPI({
        .spiBus = FSPI,
        .pinSCK = OutputPin(4),
        .pinMOSI = OutputPin(3),
        .pinMISO = InputPin::physical(2),
        .pinCS = OutputPin(5),
    });

    this->registers->add("SWS1");
    this->registers->add("SWS2");
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

    logger.info("FirmwareModuleA:started");
}

uint32_t sw9Pixel = 0;

void FirmwareModuleA::loop() {
    Firmware::loop();

    const auto& sws1Reg = this->registers->get("SWS1");
    const auto& sws2Reg = this->registers->get("SWS2");

    auto lastSws1Value = sws1Reg->read();
    auto lastSws2Value = sws2Reg->read();

    sws1Reg->write(mcp23x17->readPortA());
    sws2Reg->write(mcp23x17->readPortB());

    bool pixelsDirty = false;

    if ((sws1Reg->read() & 0x01) && !(lastSws1Value & 0x01)) {
        this->serialPort.stream().println("!switch.pressed:9");
        sw9Pixel = pixels.Color(0xff, 0x00, 0x00);
        pixelsDirty = true;
    }

    if (!(sws1Reg->read() & 0x01) && (lastSws1Value & 0x01)) {
        this->serialPort.stream().println("!switch.released:9");
        sw9Pixel = pixels.Color(0x00, 0x00, 0x00);
        pixelsDirty = true;
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
        pixels.setPixelColor(0, pixels.Color(0xff, 0x00, 0x00));
        pixels.setPixelColor(1, pixels.Color(0x00, 0xff, 0x00));
        pixels.setPixelColor(2, pixels.Color(0x00, 0x00, 0xff));
        pixels.setPixelColor(3, pixels.Color(0x00, 0xff, 0xff));
        pixels.show();
    }
}
