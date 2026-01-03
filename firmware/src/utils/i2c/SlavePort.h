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

        template <class TStruct>
        void updateEndpoint(const EndpointDescriptor<TStruct>& endpoint, const void* data) {
            std::lock_guard guard(lock);

            auto& endpointData = this->endpoints[endpoint.id];
            std::memset(&endpointData.data, 0, i2c::MAX_PACKET_SIZE);
            std::memcpy(&endpointData.data, data, endpoint.length);
            endpointData.length = endpoint.length;
        }

    private:
        void onReceiveCallback(int len);
        void onRequestCallback();

        void selectEndpoint(uint8_t endpointId);

        EndpointData endpoints[255]{};
        EndpointData& selectedEndpoint;

        TwoWire& twoWire;

        utils::allocations::Arena receiveArena;

        mutable std::mutex lock;
    };
}