#include "Firmware.h"

#include "modules/a/FirmwareModuleA.h"
#include "modules/generic/GenericFirmware.h"

std::unique_ptr<Firmware> Firmware::create(
    DeviceConfigurationManager& deviceConfigurationManager,
    SerialPort& serialPort,
    Logger& logger
    ) {

    auto deviceType = deviceConfigurationManager.getDeviceType();

    switch (deviceType) {
    case 'a':
    case 'A':
        return std::make_unique<FirmwareModuleA>(deviceConfigurationManager, serialPort, logger);
    default:
        return std::make_unique<GenericFirmware>(deviceConfigurationManager, serialPort, logger);
    }
}

Firmware::Firmware(
    DeviceConfigurationManager& deviceConfigurationManager,
    SerialPort& serialPort,
    Logger& logger
    ) :
    deviceConfigurationManager(deviceConfigurationManager),
    serialPort(serialPort),
    logger(logger) {

    this->lineStreamer = std::make_unique<LineStreamer>(serialPort.stream());
    this->commandProcessor = std::make_unique<CommandProcessor>(serialPort.stream(), logger);
    this->lineStreamer->addObserver(this->commandProcessor.get());
}


void Firmware::setup() {
}

void Firmware::loop() {
    delayMicroseconds(1000);
    lineStreamer->update();
}

void Firmware::registerCommandHandler(const std::shared_ptr<CommandHandler>& commandHandler) const {
    this->commandProcessor->addHandler(commandHandler);;
}
