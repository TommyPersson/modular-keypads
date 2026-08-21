#pragma once

#include <memory>
#include <mutex>
#include <Wire.h>

#include <tfw/utils/allocations.h>

#include "Endpoint.h"
#include "Event.h"
#include "Pins.h"
#include "tfw/hal/gpio/OutputPin.h"
#include "tfw/utils/result.h"
#include "tfw/utils/ring_queue.h"

namespace tfw::hal::i2c {
    struct EndpointData {
        uint8_t id;
        uint8_t data[MAX_PACKET_SIZE];
        uint8_t length;
    };

    class SlavePort {
    public:
        explicit SlavePort(TwoWire& twoWire);
        ~SlavePort();

        void setup(uint8_t address, Pins pins, uint8_t eventInterruptPin);

        template <class TStruct>
        void updateEndpoint(
            const EndpointDescriptor<TStruct>& endpoint,
            const void* data,
            const uint8_t length = 0 // Allows the length to be overridden for special endpoints
        ) {
            std::lock_guard guard(lock);

            auto& endpointData = this->endpoints[endpoint.id];
            std::memset(&endpointData.data, 0, i2c::MAX_PACKET_SIZE);
            std::memcpy(&endpointData.data, data, endpoint.length);

            if (length > 0) {
                endpointData.length = length;
            } else {
                endpointData.length = endpoint.length;
            }
        }

        void enqueueEvent(const Event& event);
        void triggerEventInterrupt() const;

        void addCommandHandler(void* handler);

    private:
        void selectEndpoint(uint8_t endpointId);

        EndpointData endpoints[255]{};
        void onReceiveCallback(int len);
        void onRequestCallback();

        Event* pollEvent();

        EndpointData* selectedEndpoint;

        TwoWire& twoWire;

        utils::ring_queue<Event> eventQueue{255};
        std::unique_ptr<gpio::OutputPin> eventInterruptOutputPin;

        tfw::utils::allocations::Arena receiveArena;

        std::vector<void*> knownCommandsHandlers;

        mutable std::mutex lock;
    };
}
