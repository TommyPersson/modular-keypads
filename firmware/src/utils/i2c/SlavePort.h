#pragma once

#include <mutex>
#include <Wire.h>

#include "utils/allocations/Arena.h"

#include "Endpoint.h"
#include "Pins.h"

namespace utils::i2c {
    struct EndpointData {
        uint8_t data[MAX_PACKET_SIZE];
        uint8_t length;
    };

    class SlavePort {
    public:
        explicit SlavePort(TwoWire& twoWire);
        ~SlavePort();

        void setup(uint8_t address, Pins pins);
        void updateEndpoint(const EndpointDescriptor& endpoint, const void* data);

    private:
        void onReceiveCallback(int len);
        void onRequestCallback();

        void selectEndpoint(uint8_t endpointId);

        EndpointData endpoints[10]{};
        EndpointData& selectedEndpoint;

        TwoWire& twoWire;

        utils::allocations::Arena receiveArena;

        mutable std::mutex lock;
    };
}