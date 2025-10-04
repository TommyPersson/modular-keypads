#pragma once

#include "firmwares/Firmware.h"
#include "firmwares/common/LineStreamer.h"
#include "firmwares/common/commands/CommandProcessor.h"
#include "firmwares/common/DeviceConfigurationManager.h"
#include "firmwares/common/Logger.h"

#include <vector>

#include <MCP23x17/MCP23x17.h>
#include <Registers/Register.h>
#include <SerialPort/SerialPort.h>

class FirmwareModuleA final : public Firmware {
public:
    explicit FirmwareModuleA(
        DeviceConfigurationManager& deviceConfigurationManager,
        Logger& logger
    );

    ~FirmwareModuleA() override;

    void setup() override;
    void loop() override;

private:
    DeviceConfigurationManager& deviceConfigurationManager;
    Logger& logger;

    std::unique_ptr<MCP23x17> mcp23x17;
    std::unique_ptr<SerialPort> serialPort;
    std::unique_ptr<LineStreamer> lineStreamer;
    std::unique_ptr<CommandProcessor> commandProcessor;
    std::vector<std::shared_ptr<Register>> registers;
};
