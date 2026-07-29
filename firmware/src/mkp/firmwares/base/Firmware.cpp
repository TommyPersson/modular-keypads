#include "Firmware.h"


#include <tfw/hal/logging.h>
#include "ServiceLocator.h"
#include "metrics/BaseMetrics.h"
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
    this->lineStreamer = std::make_unique<tfw::utils::streams::LineStreamer>(serialPort.stream());
    this->commandProcessor = std::make_unique<tfw::utils::commands::CommandProcessor>(serialPort.stream());
    this->lineStreamer->addObserver(this->commandProcessor.get());

    deviceFactories.emplace_back(std::make_unique<mkp::devices::a::DeviceFactoryA>());

    metrics::register_all(serviceLocator.metricRegistry);
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
    tfw::hal::time::delayUs(100);
    lineStreamer->update();
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
