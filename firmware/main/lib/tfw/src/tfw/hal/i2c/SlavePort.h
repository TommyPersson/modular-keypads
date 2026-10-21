#pragma once

#include <atomic>
#include <memory>
#include <mutex>

#ifdef ARDUINO
#include <Wire.h>
#endif

#include "Endpoint.h"
#include "Event.h"
#include "Pins.h"
#include "tfw/hal/gpio/OutputPin.h"
#include "tfw/hal/time/Clock.h"
#include "tfw/utils/result.h"
#include "tfw/utils/ring_queue.h"

namespace tfw::hal::i2c {
    #ifdef ARDUINO
    using TwoWireType = TwoWire;
    #else
    class TwoWireType;  // Forward declaration for non-Arduino builds
    #endif
    struct EndpointData {
        uint8_t id;
        uint8_t data[MAX_PACKET_SIZE];
        uint8_t length;
    };

    struct PendingCommand {
        uint8_t length = 0;
        uint8_t bytes[32]{}; // The size of the underlying Wire buffer
    };

    class SlavePort {
    public:
        SlavePort(TwoWireType& twoWire, time::Clock& clock);
        ~SlavePort();

        /**
         * Starts listening at a random temporary address, until the master assigns a permanent one. The device ID
         * tells the address assignments meant for this device apart from the ones for others at the same address.
         */
        void setup(uint64_t deviceId, Pins pins, uint8_t eventInterruptPin, uint8_t discoveryPin);

        /**
         * Drives the shared, open drain discovery line low until the master acknowledges the device, to tell the
         * master that there is a device to discover. Call once the device is ready to be scanned.
         */
        void requestDiscovery();

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

        /**
         * Executes the commands received since the last call, and resets the MCU when the master doesn't
         * acknowledge the device in time. Call from the main loop.
         */
        void loop();

    private:
        /**
         * Commands are queued by the receive callback rather than executed there, so that handlers run on the
         * main loop.
         */
        void processPendingCommands();

        void onDiscoveryTimeout();

        void selectEndpoint(uint8_t endpointId);

        /**
         * Returns whether the command was one of the address commands, which are handled here rather than by the
         * added command handlers.
         */
        bool processAddressCommand(const PendingCommand& command);
        void listenAt(uint8_t address);
        static uint8_t randomTemporaryAddress();

        EndpointData endpoints[255]{};
        void onReceiveCallback(int len);
        void onRequestCallback();

        Event* pollEvent();

        EndpointData* selectedEndpoint;

        TwoWireType& twoWire;
        Pins pins{};
        uint64_t deviceId = 0;
        uint8_t currentAddress = 0;

        utils::ring_queue<Event> eventQueue{255};
        std::unique_ptr<gpio::OutputPin> eventInterruptOutputPin;
        std::unique_ptr<gpio::OutputPin> discoveryOutputPin;

        utils::ring_queue<PendingCommand> pendingCommands{16};

        // Set from the receive callback, which runs on the Wire task
        std::atomic<bool> discoveryAcknowledged{false};
        std::unique_ptr<time::CountDown> discoveryTimeout;

        std::vector<void*> knownCommandsHandlers;

        mutable std::mutex lock;
    };
}
