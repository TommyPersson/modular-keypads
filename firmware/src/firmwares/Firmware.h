#pragma once

#include <Wire.h>
#include <memory>
#include <Registers/Registers.h>
#include <SerialPort/SerialPort.h>

#include "common/DeviceConfigurationManager.h"
#include "common/LineStreamer.h"
#include "common/ServiceLocator.h"
#include "common/commands/CommandProcessor.h"
#include "common/notifications/Notifier.h"

class Firmware {
public:
    explicit Firmware(ServiceLocator& serviceLocator);

    virtual ~Firmware() = default;

    virtual void setup();
    virtual void loop();

    static std::unique_ptr<Firmware> create(ServiceLocator& serviceLocator);

protected:
    void addCommandHandler(const std::shared_ptr<CommandHandler>& commandHandler) const;
    std::shared_ptr<Register> addRegister(const std::string& name) const;

    DeviceConfigurationManager& deviceConfigurationManager;
    SerialPort& serialPort;
    Notifier& notifier;
    Logger& logger;
    TwoWire& i2c;

    std::unique_ptr<RegisterManager> registers;

private:
    std::unique_ptr<LineStreamer> lineStreamer;
    std::unique_ptr<CommandProcessor> commandProcessor;
};
