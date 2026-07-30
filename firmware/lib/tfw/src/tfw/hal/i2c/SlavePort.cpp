#include "SlavePort.h"

#include <cstring>

#include "Commands.h"
#include "Pins.h"
#include <tfw/hal/logging.h>

#include "tfw/hal/gpio/OutputPin.h"

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

    void SlavePort::setup(const uint8_t address, Pins pins, const uint8_t eventInterruptPin) {
        twoWire.onReceive([this](const int len) { onReceiveCallback(len); });
        twoWire.onRequest([this]() { onRequestCallback(); });
        twoWire.begin(address, pins.SDA, pins.SCL, 100'000);

        addCommandHandler(
            new commands::LambdaRemoteCommandHandler<commands::builtin::SetEndpointParams>(
                commands::builtin::SetEndpoint.id,
                [this](const commands::builtin::SetEndpointParams& params) -> utils::void_result {
                    this->selectEndpoint(params.endpointId);
                    return utils::void_result::success();
                }
            )
        );

        eventInterruptOutputPin = gpio::OutputPin::physical(eventInterruptPin, OPEN_DRAIN);
        eventInterruptOutputPin->init();
    }

    void SlavePort::addCommandHandler(void* handler) {
        knownCommandsHandlers.push_back(handler);
    }

    void SlavePort::onReceiveCallback(const int len) {
        std::lock_guard guard(lock);

        auto messageBytes = receiveArena.allocate(len);

        int i = 0;
        while (Wire.available()) {
            int value = Wire.read();

            if (i == 0 && value == 0x17) {
                continue;
            }

            messageBytes[i++] = value;
        }

        for (auto& commandPtr : knownCommandsHandlers) {
            const auto command = static_cast<commands::RemoteCommandHandler<commands::AnyParams>*>(commandPtr);
            const auto message = command->parseMessage(messageBytes);
            if (message->commandId != command->id) {
                continue;
            }

            command->execute(&message->params); // TODO what to do with returned errors?
        }

        receiveArena.reset();
    }

    void SlavePort::onRequestCallback() {
        std::lock_guard guard(lock);

        if (selectedEndpoint.id == i2c::endpoints::builtin::Events.id) {
            const auto event = this->pollEvent();
            if (event == nullptr) {
                constexpr auto emptyEvent = Event{};
                const auto bytes = reinterpret_cast<const uint8_t*>(&emptyEvent);
                twoWire.write(bytes, sizeof(emptyEvent));
                return;
            }

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
        eventQueue.enqueue(event);
    }

    void SlavePort::triggerEventInterrupt() const {
        eventInterruptOutputPin->setLow();
        eventInterruptOutputPin->setHigh();
    }

    Event* SlavePort::pollEvent() {
        return eventQueue.dequeue();
    }
}
