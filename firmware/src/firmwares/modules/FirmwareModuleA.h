#pragma once

#include "../Firmware.h"

#include <vector>

#include <MCP23x17/MCP23x17.h>
#include <SerialPort/SerialPort.h>
#include <Registers/Register.h>


class FirmwareModuleA final : public Firmware {
public:
    FirmwareModuleA();
    ~FirmwareModuleA() override;
    void setup() override;
    void loop() override;

private:
    std::unique_ptr<MCP23x17> mcp23x17;
    std::unique_ptr<SerialPort> serialPort;
    std::vector<std::shared_ptr<Register>> registers;
};
