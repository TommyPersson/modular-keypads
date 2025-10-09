#include <Arduino.h>

#include <USB.h>
#include <SPI.h>

#include <cstdint>

//#include <MacroPad/MacroPadHardware.h>
//#include <MacroPad/MacroPad.h>

//USBHIDKeyboard keyboard;
//USBHIDConsumerControl consumerControl;
//USBHIDSystemControl systemControl;

#include <SerialPort/SerialPort.h>
#include <firmwares/common/DeviceConfigurationManager.h>
#include <firmwares/modules/a/FirmwareModuleA.h>

#include <Preferences.h>

std::unique_ptr<Firmware> firmware;

Preferences preferences;
Logger logger(Serial);
std::unique_ptr<SerialPort> serialPort = SerialPort::from(Serial);
DeviceConfigurationManager deviceConfigurationManager(preferences, logger);

void setup() {
    serialPort->begin(115200);
    deviceConfigurationManager.begin();

    firmware = Firmware::create(deviceConfigurationManager, *serialPort, logger);
    firmware->setup();

  //  USB.productName("tommy-product");
  //  USB.manufacturerName("tommy-manufacturer");

}

void loop() {
    firmware->loop();
}
