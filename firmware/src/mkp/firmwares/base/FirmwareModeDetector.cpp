#include "FirmwareModeDetector.h"


using namespace mkp::firmwares::base;

void FirmwareModeDetector::setup() {
    vbusPin = tfw::hal::gpio::InputPin::physical(8);
}

FirmwareMode FirmwareModeDetector::detectFirmwareMode() const {
    const auto voltageReading = vbusPin->readAnalog();
    if (voltageReading >= 2000) {
        return FirmwareMode::Master;
    }

    return FirmwareMode::Slave;
}
