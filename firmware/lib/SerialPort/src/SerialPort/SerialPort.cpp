#include "SerialPort.h"

#include <memory>

#include "HardwareSerialPort.h"
#include "HWCDCSerialPort.h"

#if SOC_USB_OTG_SUPPORTED
#include "UsbSerialPort.h"
#endif

namespace utils::serial {
	std::unique_ptr<SerialPort> SerialPort::from(HardwareSerial& serial) {
		return std::make_unique<HardwareSerialPort>(serial);
	}

#if SOC_USB_OTG_SUPPORTED
	std::unique_ptr<SerialPort> SerialPort::from(USBCDC& serial) {
		return std::make_unique<UsbSerialPort>(serial);
	}
#endif

	std::unique_ptr<SerialPort> SerialPort::from(HWCDC& serial) {
		return std::make_unique<HWCDCSerialPort>(serial);
	}
}