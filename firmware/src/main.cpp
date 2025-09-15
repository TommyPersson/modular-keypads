#include <Arduino.h>

#include <USB.h>
#include <SPI.h>

#include <cstdint>

//#include <MacroPad/MacroPadHardware.h>
//#include <MacroPad/MacroPad.h>

//USBHIDKeyboard keyboard;
//USBHIDConsumerControl consumerControl;
//USBHIDSystemControl systemControl;

#include <firmwares/modules/FirmwareModuleA.h>

std::unique_ptr<Firmware> firmware;

void setup() {
    firmware = Firmware::create('A');
    firmware->setup();

  //  USB.productName("tommy-product");
  //  USB.manufacturerName("tommy-manufacturer");

}

void loop() {
    firmware->loop();
}
