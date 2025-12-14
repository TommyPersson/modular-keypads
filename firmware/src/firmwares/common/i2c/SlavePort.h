#pragma once

#include <Wire.h>

#include "utils/allocations/Arena.h"

#include "Endpoint.h"

namespace i2c {
    struct EndpointData {
        uint8_t data[32];
        uint8_t length;
    };

    class SlavePort {
    public:
        explicit SlavePort(TwoWire& twoWire);
        ~SlavePort();

        void begin(uint8_t address, uint8_t sdaPin, uint8_t sclPin);
        void updateEndpoint(Endpoint endpoint, const void* data, uint8_t length);

    private:
        void onReceiveCallback(int len);
        void onRequestCallback();

        void selectEndpoint(Endpoint endpoint);

        EndpointData endpoints[10]{};
        EndpointData& selectedEndpoint;

        TwoWire& twoWire;

        Arena receiveArena;
    };
}
