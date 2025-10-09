#pragma once

#include <memory>
#include <SerialPort/SerialPort.h>

#include "common/DeviceConfigurationManager.h"
#include "common/LineStreamer.h"
#include "common/commands/CommandProcessor.h"

class Firmware {
public:
    Firmware(
        DeviceConfigurationManager& deviceConfigurationManager,
        SerialPort& serialPort,
        Logger& logger
    );

    virtual ~Firmware() = default;

    virtual void setup();
    virtual void loop();

    static std::unique_ptr<Firmware> create(
        DeviceConfigurationManager& deviceConfigurationManager,
        SerialPort& serialPort,
        Logger& logger
    );

protected:
    void registerCommandHandler(const std::shared_ptr<CommandHandler>& commandHandler) const;

    DeviceConfigurationManager& deviceConfigurationManager;
    SerialPort& serialPort;
    Logger& logger;

private:
    std::unique_ptr<LineStreamer> lineStreamer;
    std::unique_ptr<CommandProcessor> commandProcessor;
};
