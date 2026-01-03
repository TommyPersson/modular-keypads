#include "SlavePort.h"

#include <cstring>

#include "Operation.h"
#include "Pins.h"
#include "utils/logging/Logger.h"

namespace {
    auto logger = utils::logging::createLogger("SlavePort");
}

namespace utils::i2c {
    SlavePort::SlavePort(TwoWire& twoWire) :
        selectedEndpoint(endpoints[0]),
        twoWire(twoWire),
        receiveArena(32) {
    }

    SlavePort::~SlavePort() = default;

    void SlavePort::setup(const uint8_t address, Pins pins) {
        twoWire.onReceive([this](const int len) { onReceiveCallback(len); });
        twoWire.onRequest([this]() { onRequestCallback(); });
        twoWire.begin(address, pins.SDA, pins.SCL, 100'000);
    }

    void SlavePort::updateEndpoint(const EndpointDescriptor& endpoint, const void* data) {
        std::lock_guard guard(lock);

        auto& endpointData = this->endpoints[endpoint.id];
        std::memset(&endpointData.data, 0, i2c::MAX_PACKET_SIZE);
        std::memcpy(&endpointData.data, data, endpoint.length);
        endpointData.length = endpoint.length;
    }

    void SlavePort::onReceiveCallback(const int len) {
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
            const auto endpoint = data[1];
            selectEndpoint(endpoint);
        }

        receiveArena.reset();
    }

    void SlavePort::onRequestCallback() {
        std::lock_guard guard(lock);

        twoWire.write(selectedEndpoint.data, selectedEndpoint.length);
    }

    void SlavePort::selectEndpoint(const uint8_t endpointId) {
        selectedEndpoint = this->endpoints[endpointId];
    }
}