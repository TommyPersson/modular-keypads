#pragma once

#include <memory>
#include <optional>
#include <Wire.h>
#include <tfw/hal/uart.h>
#include <tfw/utils/streams.h>

#include "../common/DeviceConfigurationManager.h"
#include "../common/ServiceLocator.h"

#include <tfw/utils/commands.h>

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
    void addCommandHandler(const std::shared_ptr<tfw::utils::commands::CommandHandler>& commandHandler) const;
    devices::DeviceModuleFactory* getModuleFactory(char deviceType) const;

    void registerMetrics();

    DeviceConfigurationManager& deviceConfigurationManager;
    tfw::utils::serial::SerialPort& serialPort;
    ServiceLocator& serviceLocator;

    std::unique_ptr<devices::DeviceModule> deviceModule;
    std::optional<tfw::utils::registers::RegisterManager*> registers;

private:
    std::unique_ptr<tfw::utils::streams::LineStreamer> lineStreamer;
    std::unique_ptr<tfw::utils::commands::CommandProcessor> commandProcessor;
    std::vector<std::unique_ptr<devices::DeviceModuleFactory>> moduleFactories;
};
