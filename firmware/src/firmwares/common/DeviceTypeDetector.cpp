#include "DeviceTypeDetector.h"

using namespace devices;

DeviceTypeDetector::DeviceTypeDetector() {
    shiftRegister = std::make_unique<tfw::ic::L74165>(
        tfw::ic::Config{
            .pinQ = tfw::utils::gpio::InputPin::physical(18),
            .pinCLK = tfw::utils::gpio::OutputPin::physical(17),
            .pinCE = tfw::utils::gpio::OutputPin::physical(15),
            .pinLD = tfw::utils::gpio::OutputPin::physical(16),
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
