#include "DeviceTypeDetector.h"

using namespace devices;

DeviceTypeDetector::DeviceTypeDetector() {
    shiftRegister = std::make_unique<chips::L74165::L74165>(
        chips::L74165::Config{
            .pinQ = utils::pins::InputPin::physical(18),
            .pinCLK = utils::pins::OutputPin::physical(17),
            .pinCE = utils::pins::OutputPin::physical(15),
            .pinLD = utils::pins::OutputPin::physical(16),
        }
    );
}

void DeviceTypeDetector::setup() const {
    shiftRegister->setup();
}

char DeviceTypeDetector::detectDeviceType() const {
    shiftRegister->parallelLoad();
    const uint8_t typeCode = shiftRegister->read();

    switch (typeCode) {
    case 0x01: return 'a';
    default: return 'g';
    }
}
