#pragma once

#include <Wire.h>

#include "utils/allocations/Arena.h"

#include "Endpoint.h"
#include "I2cPins.h"

namespace i2c {
    struct EndpointData {
        uint8_t data[MAX_PACKET_SIZE];
        uint8_t length;
    };

    class SlavePort {
    public:
        explicit SlavePort(TwoWire& twoWire);
        ~SlavePort();

        void setup(uint8_t address, Pins pins);
        void updateEndpoint(Endpoint endpoint, const void* data, uint8_t length);

    private:
        void onReceiveCallback(int len);
        void onRequestCallback();

        void selectEndpoint(Endpoint endpoint);

        EndpointData endpoints[10]{};
        EndpointData& selectedEndpoint;

        TwoWire& twoWire;

        Arena receiveArena;

        mutable std::mutex lock;
    };
}
