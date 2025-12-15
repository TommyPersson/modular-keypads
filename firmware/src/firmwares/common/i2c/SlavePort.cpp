#include "SlavePort.h"

#include <cstring>

#include "I2cPins.h"
#include "Operation.h"

namespace {
    auto logger = common::logging::createLogger("SlavePort");
}

i2c::SlavePort::SlavePort(TwoWire& twoWire) :
    selectedEndpoint(endpoints[0]),
    twoWire(twoWire),
    receiveArena(32) {
}

i2c::SlavePort::~SlavePort() = default;

void i2c::SlavePort::setup(const uint8_t address, Pins pins) {
    twoWire.onReceive([this](const int len) { onReceiveCallback(len); });
    twoWire.onRequest([this]() { onRequestCallback(); });
    twoWire.begin(address, pins.SDA, pins.SCL, 100'000);
}

void i2c::SlavePort::updateEndpoint(Endpoint endpoint, const void* data, uint8_t length) {
    std::lock_guard guard(lock);

    auto& endpointData = this->endpoints[static_cast<int>(endpoint)];
    std::memset(&endpointData.data, 0, i2c::MAX_PACKET_SIZE);
    std::memcpy(&endpointData.data, data, length);
    endpointData.length = length;
}

void i2c::SlavePort::onReceiveCallback(const int len) {
    std::lock_guard guard(lock);

    auto data = receiveArena.allocate(len);

    int i = 0;
    while (Wire.available()) {
        int value = Wire.read();

        if (i == 0 && value == 0x17) {
            continue;
        }

        data[i++] = value;
    }

    if (data[0] == static_cast<int>(Operation::SetEndpoint)) {
        const auto endpoint = static_cast<Endpoint>(data[1]);
        selectEndpoint(endpoint);
    }

    receiveArena.reset();
}

void i2c::SlavePort::onRequestCallback() {
    std::lock_guard guard(lock);

    twoWire.write(selectedEndpoint.data, selectedEndpoint.length);
}

void i2c::SlavePort::selectEndpoint(Endpoint endpoint) {
    selectedEndpoint = this->endpoints[static_cast<int>(endpoint)];
}
