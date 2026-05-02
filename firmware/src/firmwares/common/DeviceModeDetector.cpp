#include "DeviceModeDetector.h"

void devices::DeviceModeDetector::setup() {
    vbusPin = tfw::hal::gpio::InputPin::physical(8);
}

devices::DeviceMode devices::DeviceModeDetector::detectDeviceMode() const {
    const auto voltageReading = vbusPin->readAnalog();
    if (voltageReading >= 2000) {
        return DeviceMode::Master;
    }

    return DeviceMode::Slave;
}
