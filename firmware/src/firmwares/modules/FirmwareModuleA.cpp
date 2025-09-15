#include "FirmwareModuleA.h"

#include <SPI.h>
#include <Adafruit_NeoPixel.h>

FirmwareModuleA::FirmwareModuleA() {
    this->mcp23x17 = MCP23x17::SPI({
        .spiBus = FSPI,
        .pinSCK = OutputPin(4),
        .pinMOSI = OutputPin(3),
        .pinMISO = InputPin::physical(2),
        .pinCS = OutputPin(5),
    });

    this->serialPort = SerialPort::from(Serial);

    this->registers.push_back(std::make_shared<Register>("SWS1"));
    this->registers.push_back(std::make_shared<Register>("SWS2"));
}

FirmwareModuleA::~FirmwareModuleA() {
}

int i = 0;
int j = 0;
Adafruit_NeoPixel pixels(12, 6, NEO_GRB + NEO_KHZ800);

void FirmwareModuleA::setup() {
    i = 0;
    j = 0;

    serialPort->begin(115200);
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
    pixels.setBrightness(100);

    serialPort->stream().printf("started\n");
}

void FirmwareModuleA::loop() {
    delayMicroseconds(1000);

    const auto& sws1Reg = this->registers.at(0);
    const auto& sws2Reg = this->registers.at(1);

    sws1Reg->write(mcp23x17->readPortA());
    sws2Reg->write(mcp23x17->readPortB());

    i++;
    if (i == 1000) {
        j++;
        i = 0;

        uint8_t gppua = mcp23x17->readRegister(MCP23x17_Registers::GPPUA);
        uint8_t gppub = mcp23x17->readRegister(MCP23x17_Registers::GPPUB);

        uint8_t iodira = mcp23x17->readRegister(MCP23x17_Registers::IODIRA);
        uint8_t iodirb = mcp23x17->readRegister(MCP23x17_Registers::IODIRB);

        serialPort->stream().printf(
            "%4i: SWS1=%2x, SWS2=%2x, DIRA=%2x, DIRB=%2x, GPPUA=%2x, GPPUB=%2x\n",
            j,
            sws1Reg->read(),
            sws2Reg->read(),
            iodira,
            iodirb,
            gppua,
            gppub
            );

        pixels.clear();
        pixels.setPixelColor(0, pixels.Color(0xff, 0x00, 0x00));
        pixels.setPixelColor(1, pixels.Color(0x00, 0xff, 0x00));
        pixels.setPixelColor(2, pixels.Color(0x00, 0x00, 0xff));
        pixels.setPixelColor(3, pixels.Color(0x00, 0xff, 0xff));
        pixels.setPixelColor(4, pixels.Color(0xff, 0xff, 0xff));
        pixels.show();
    }
}
