#include <Arduino.h>

#include <SPI.h>
#include <USB.h>

#include <cstdint>
#include <memory>

//#include <MacroPad/MacroPadHardware.h>
//#include <MacroPad/MacroPad.h>

//USBHIDKeyboard keyboard;
//USBHIDConsumerControl consumerControl;
//USBHIDSystemControl systemControl;

#include <Preferences.h>
#include <Wire.h>

#include <SerialPort/SerialPort.h>

#include "firmwares/Firmware.h"
#include "firmwares/common/DeviceConfigurationManager.h"
#include "firmwares/common/ServiceLocator.h"
#include "firmwares/common/logging/Logger.h"
#include "firmwares/common/notifications/NotifierFactory.h"


std::unique_ptr<Firmware> firmware;
std::unique_ptr<Preferences> preferences;
std::unique_ptr<SerialPort> serialPort;
std::unique_ptr<NotifierFactory> notifierFactory;
std::unique_ptr<DeviceConfigurationManager> deviceConfigurationManager;
std::unique_ptr<I2cClient> i2cClient;
std::unique_ptr<i2c::SlavePort> i2cSlavePort;

std::unique_ptr<ServiceLocator> serviceLocator;

void setup() {
    common::logging::initialize(&Serial);

    serialPort = SerialPort::from(Serial);
    preferences = std::make_unique<Preferences>();
    deviceConfigurationManager = std::make_unique<DeviceConfigurationManager>(*preferences);
    notifierFactory = std::make_unique<NotifierFactory>(Serial);
    i2cClient = std::make_unique<I2cClient>(Wire);
    i2cSlavePort = std::make_unique<i2c::SlavePort>(Wire);

    serviceLocator = std::make_unique<ServiceLocator>(ServiceLocator{
        .deviceConfigurationManager = *deviceConfigurationManager,
        .serialPort = *serialPort,
        .notifierFactory = *notifierFactory,
        .i2cClient = *i2cClient,
        .i2cSlavePort = *i2cSlavePort,
        .i2c = Wire,
    });

    firmware = Firmware::create(*serviceLocator);
    firmware->setup();

  //  USB.productName("tommy-product");
  //  USB.manufacturerName("tommy-manufacturer");

}

void loop() {
    firmware->loop();
}
