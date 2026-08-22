#include "Firmware.h"

#include <tfw/hal/streams/InputStream.h>
#include <tfw/hal/streams/OutputStream.h>
#include <tfw/hal/logging.h>
#include "ServiceLocator.h"
#include "mkp/components/metrics/BaseMetrics.h"
#include "mkp/devices/a/DeviceFactoryA.h"
#include "mkp/firmwares//slave/SlaveFirmware.h"
#include "mkp/firmwares/master/MasterFirmware.h"


namespace {
    auto logger = tfw::hal::logging::createLogger("Firmware");
}

using namespace mkp::firmwares::base;

Firmware::Firmware(ServiceLocator& serviceLocator) :
    deviceConfigurationManager(serviceLocator.deviceConfigurationManager),
    serialPort(serviceLocator.serialPort),
    serviceLocator(serviceLocator) {
    this->inputStream = std::unique_ptr<tfw::hal::streams::InputStream>(
        tfw::hal::streams::createArduinoInputStream(serialPort.stream())
    );
    this->outputStream = std::unique_ptr<tfw::hal::streams::OutputStream>(
        tfw::hal::streams::createArduinoOutputStream(serialPort.stream())
    );

    this->lineStreamer = std::make_unique<tfw::utils::streams::LineStreamer>(*this->inputStream);
    this->commandProcessor = std::make_unique<tfw::utils::commands::CommandProcessor>(*this->outputStream);
    this->lineStreamer->addObserver(this->commandProcessor.get());

    deviceFactories.emplace_back(std::make_unique<mkp::devices::a::DeviceFactoryA>());

    components::metrics::register_all(serviceLocator.metricRegistry);
}

mkp::devices::common::DeviceFactory* Firmware::getDeviceFactory(char deviceType) const {
    for (const auto& factory : deviceFactories) {
        if (factory->matches(deviceType)) {
            return factory.get();
        }
    }

    return nullptr;
}

void Firmware::setup() {
    serialPort.begin(115200);
    serviceLocator.usbConnection.setup();
    deviceConfigurationManager.begin();
}

void Firmware::loop() {
    serviceLocator.clock.delayUs(100);
    lineStreamer->update();
    serviceLocator.usbConnection.update();
}

void Firmware::addCommandHandler(const std::shared_ptr<tfw::utils::commands::CommandHandler>& commandHandler) const {
    this->commandProcessor->addHandler(commandHandler);
}

std::unique_ptr<Firmware> Firmware::create(ServiceLocator& serviceLocator) {
    const auto deviceMode = serviceLocator.deviceModeDetector.detectFirmwareMode();

    if (deviceMode == FirmwareMode::Master) {
        return std::make_unique<MasterFirmware>(serviceLocator);
    }

    return std::make_unique<SlaveFirmware>(serviceLocator);
}
