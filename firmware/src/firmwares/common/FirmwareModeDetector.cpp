#include "FirmwareModeDetector.h"

void devices::FirmwareModeDetector::setup() {
    vbusPin = tfw::hal::gpio::InputPin::physical(8);
}

devices::FirmwareMode devices::FirmwareModeDetector::detectFirmwareMode() const {
    const auto voltageReading = vbusPin->readAnalog();
    if (voltageReading >= 2000) {
        return FirmwareMode::Master;
    }

    return FirmwareMode::Slave;
}
