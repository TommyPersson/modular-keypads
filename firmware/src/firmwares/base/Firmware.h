#pragma once

#include <Wire.h>
#include <memory>
#include <SerialPort/SerialPort.h>

#include "../common/DeviceConfigurationManager.h"
#include "../common/LineStreamer.h"
#include "../common/ServiceLocator.h"
#include "../common/commands/CommandProcessor.h"

#include "firmwares/modules/common/DeviceModule.h"
#include "firmwares/modules/common/DeviceModuleFactory.h"

class Firmware {
public:
    explicit Firmware(ServiceLocator& serviceLocator);

    virtual ~Firmware() = default;

    virtual void setup();
    virtual void loop();

    static std::unique_ptr<Firmware> create(ServiceLocator& serviceLocator);

protected:
    void addCommandHandler(const std::shared_ptr<CommandHandler>& commandHandler) const;
    devices::DeviceModuleFactory* getModuleFactory(char deviceType) const;

    DeviceConfigurationManager& deviceConfigurationManager;
    SerialPort& serialPort;
    TwoWire& i2c;
    ServiceLocator& serviceLocator;

    std::unique_ptr<devices::DeviceModule> deviceModule;
    std::optional<RegisterManager*> registers;

private:
    std::unique_ptr<LineStreamer> lineStreamer;
    std::unique_ptr<CommandProcessor> commandProcessor;
    std::vector<std::unique_ptr<devices::DeviceModuleFactory>> moduleFactories;
};
