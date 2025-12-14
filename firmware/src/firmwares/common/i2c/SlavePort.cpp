#include "SlavePort.h"

#include <cstring>

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

void i2c::SlavePort::begin(const uint8_t address, const uint8_t sdaPin, const uint8_t sclPin) {
    twoWire.onReceive([this](const int len) { onReceiveCallback(len); });
    twoWire.onRequest([this]() { onRequestCallback(); });
    twoWire.begin(address, sdaPin, sclPin, 100'000);
}

void i2c::SlavePort::updateEndpoint(Endpoint endpoint, const void* data, uint8_t length) {
    auto& endpointData = this->endpoints[static_cast<int>(endpoint)];
    std::memset(&endpointData.data, 0, 32);
    std::memcpy(&endpointData.data, data, length);
    endpointData.length = length;
}

void i2c::SlavePort::onReceiveCallback(const int len) {
    auto data = receiveArena.allocate(len);

    int i = 0;
    while (Wire.available()) {
        data[i++] = Wire.read();
    }

    if (data[0] == static_cast<int>(Operation::SetEndpoint)) {
        const auto endpoint = static_cast<Endpoint>(data[1]);
        selectEndpoint(endpoint);
    }

    receiveArena.reset();
}

void i2c::SlavePort::onRequestCallback() {
    twoWire.write(selectedEndpoint.data, selectedEndpoint.length);
}

void i2c::SlavePort::selectEndpoint(Endpoint endpoint) {
    selectedEndpoint = this->endpoints[static_cast<int>(endpoint)];

    logger->info("selected endpoint = %i", static_cast<int>(endpoint));
}
