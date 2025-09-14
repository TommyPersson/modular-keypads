#include "SerialPort.h"

#include <memory>

#include "HardwareSerialPort.h"
#include "UsbSerialPort.h"

std::unique_ptr<SerialPort> SerialPort::from(HardwareSerial& serial) {
	return std::make_unique<HardwareSerialPort>(serial);
}

std::unique_ptr<SerialPort> SerialPort::from(USBCDC& serial) {
	return std::make_unique<UsbSerialPort>(serial);
}