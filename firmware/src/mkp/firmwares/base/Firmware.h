#pragma once

#include <memory>
#include <optional>
#include <Wire.h>
#include <tfw/hal/uart.h>
#include <tfw/utils/streams.h>
#include <tfw/utils/commands.h>

#include "ServiceLocator.h"
#include "mkp/devices/common/DeviceConfigurationManager.h"
#include "mkp/devices/common/DeviceFactory.h"
#include "mkp/devices/common/LocalDevice.h"


class Firmware {
public:
    explicit Firmware(ServiceLocator& serviceLocator);

    virtual ~Firmware() = default;

    virtual void setup();
    virtual void loop();

    static std::unique_ptr<Firmware> create(ServiceLocator& serviceLocator);

protected:
    void addCommandHandler(const std::shared_ptr<tfw::utils::commands::CommandHandler>& commandHandler) const;
    mkp::devices::common::DeviceFactory* getDeviceFactory(char deviceType) const;

    void registerMetrics();

    mkp::devices::common::DeviceConfigurationManager& deviceConfigurationManager;
    tfw::hal::uart::SerialPort& serialPort;
    ServiceLocator& serviceLocator;

    std::unique_ptr<mkp::devices::common::LocalDevice> deviceModule;
    std::optional<tfw::utils::registers::RegisterManager*> registers;

private:
    std::unique_ptr<tfw::utils::streams::LineStreamer> lineStreamer;
    std::unique_ptr<tfw::utils::commands::CommandProcessor> commandProcessor;
    std::vector<std::unique_ptr<mkp::devices::common::DeviceFactory>> deviceFactories;
};
