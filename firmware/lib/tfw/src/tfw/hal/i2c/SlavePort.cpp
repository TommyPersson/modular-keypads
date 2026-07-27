#include "SlavePort.h"

#include <cstring>

#include "Commands.h"
#include "Pins.h"
#include <tfw/hal/logging.h>

namespace {
    auto logger = tfw::hal::logging::createLogger("SlavePort");
}

namespace tfw::hal::i2c {
    SlavePort::SlavePort(TwoWire& twoWire) :
        selectedEndpoint(endpoints[0]),
        twoWire(twoWire),
        receiveArena(32) {

        uint8_t i = 0;
        for (auto& endpoint : endpoints) {
            endpoint.id = i;
            i++;
        }
    }

    SlavePort::~SlavePort() = default;

    void SlavePort::setup(const uint8_t address, Pins pins) {
        twoWire.onReceive([this](const int len) { onReceiveCallback(len); });
        twoWire.onRequest([this]() { onRequestCallback(); });
        twoWire.begin(address, pins.SDA, pins.SCL, 100'000);

        addCommandHandler(new commands::LambdaRemoteCommandHandler<commands::builtin::SetEndpointParams>(
            commands::builtin::SetEndpoint.id,
            [this](const commands::builtin::SetEndpointParams& params) -> utils::void_result {
                this->selectEndpoint(params.endpointId);
                return utils::void_result::success();
            }));
    }

    void SlavePort::addCommandHandler(void* handler) {
        knownCommandsHandlers.push_back(handler);
    }

    void SlavePort::onReceiveCallback(const int len) {
        std::lock_guard guard(lock);

        auto message = receiveArena.allocate(len);

        int i = 0;
        while (Wire.available()) {
            int value = Wire.read();

            if (i == 0 && value == 0x17) {
                continue;
            }

            message[i++] = value;
        }

        for (auto& commandPtr : knownCommandsHandlers) {
            const auto command = static_cast<commands::RemoteCommandHandler<commands::AnyParams>*>(commandPtr);
            if (command->id != message[0]) {
                continue;
            }
            const auto params = command->parseData(message);
            command->execute(params); // TODO what to do with returned errors?
        }

        receiveArena.reset();
    }

    void SlavePort::onRequestCallback() {
        std::lock_guard guard(lock);

        if (selectedEndpoint.id == i2c::endpoints::builtin::Events.id) {
            const auto event = this->pollEvent();
            if (event == nullptr) {
                constexpr auto emptyEvent = Event{};
                //constexpr auto emptyEvent = Event{ .type = 0x43};
                const auto bytes = reinterpret_cast<const uint8_t*>(&emptyEvent);
                twoWire.write(bytes, sizeof(emptyEvent));
                return;
            }

            /*constexpr auto emptyEvent = Event{ .type = 0x42};
            const auto bytes = reinterpret_cast<const uint8_t*>(&emptyEvent);
            twoWire.write(bytes, sizeof(emptyEvent));*/

            const auto bytes = reinterpret_cast<const uint8_t*>(event);
            twoWire.write(bytes, sizeof(*event));
            return;
        }

        twoWire.write(selectedEndpoint.data, selectedEndpoint.length);
    }

    void SlavePort::selectEndpoint(const uint8_t endpointId) {
        selectedEndpoint = this->endpoints[endpointId];
    }

    void SlavePort::enqueueEvent(const Event& event) {
        this->eventQueue[eventProducerIndex] = event;

        eventProducerIndex++;
        if (eventProducerIndex >= 255) {
            eventProducerIndex = 0;
        }
    }

    Event* SlavePort::pollEvent() {
        if (eventConsumerIndex == eventProducerIndex) {
            return nullptr;
        }

        auto& event = this->eventQueue[eventConsumerIndex];

        eventConsumerIndex++;
        if (eventConsumerIndex >= 255) {
            eventConsumerIndex = 0;
        }

        return &event;
    }
}
