#pragma once

#include "firmwares/Firmware.h"
#include "firmwares/common/DeviceConfigurationManager.h"
#include "firmwares/common/Logger.h"

#include <MCP23x17/MCP23x17.h>
#include <SerialPort/SerialPort.h>

class FirmwareModuleA final : public Firmware {
public:
    explicit FirmwareModuleA(
        DeviceConfigurationManager& deviceConfigurationManager,
        SerialPort& serialPort,
        Logger& logger
    );

    ~FirmwareModuleA() override;

    void setup() override;
    void loop() override;

private:
    std::unique_ptr<MCP23x17> mcp23x17;
};
