#pragma once

#include <memory>
#include <Registers/Registers.h>
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
    void addCommandHandler(const std::shared_ptr<CommandHandler>& commandHandler) const;
    void addRegister(const std::string& name) const;

    DeviceConfigurationManager& deviceConfigurationManager;
    SerialPort& serialPort;
    Logger& logger;

    std::unique_ptr<RegisterManager> registers;

private:
    std::unique_ptr<LineStreamer> lineStreamer;
    std::unique_ptr<CommandProcessor> commandProcessor;
};
