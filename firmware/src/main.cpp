#include <Arduino.h>

#include <USB.h>
#include <SPI.h>

#include <cstdint>

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


std::unique_ptr<Firmware> firmware;

Preferences preferences;
Logger logger(Serial);
std::unique_ptr<SerialPort> serialPort = SerialPort::from(Serial);
DeviceConfigurationManager deviceConfigurationManager(preferences, logger);
Notifier notifier(Serial);

void setup() {
    firmware = Firmware::create(deviceConfigurationManager, *serialPort, notifier, logger, Wire);
    firmware->setup();

  //  USB.productName("tommy-product");
  //  USB.manufacturerName("tommy-manufacturer");

}

void loop() {
    firmware->loop();
}
