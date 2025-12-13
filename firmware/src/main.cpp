#include <Arduino.h>

#include <USB.h>
#include <SPI.h>

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

#include "firmwares/common/notifications/Notifier.h"
#include "firmwares/common/DeviceConfigurationManager.h"
#include "firmwares/Firmware.h"
#include "firmwares/common/ServiceLocator.h"


std::unique_ptr<Firmware> firmware;

Preferences preferences;
Logger logger(Serial);
std::unique_ptr<SerialPort> serialPort = SerialPort::from(Serial);
DeviceConfigurationManager deviceConfigurationManager(preferences, logger);
Notifier notifier(Serial);

std::unique_ptr<ServiceLocator> serviceLocator;

void setup() {
    serviceLocator = std::make_unique<ServiceLocator>(ServiceLocator{
        .deviceConfigurationManager = deviceConfigurationManager,
        .serialPort = *serialPort,
        .notifier = notifier,
        .logger = logger,
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
