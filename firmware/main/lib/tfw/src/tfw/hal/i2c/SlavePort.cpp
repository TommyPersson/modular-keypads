#include "SlavePort.h"

#ifdef ARDUINO

#include <cstring>
#include <Wire.h>

#include <tfw/hal/logging.h>
#include "Commands.h"
#include "Pins.h"

#include "tfw/hal/gpio/OutputPin.h"

namespace {
    auto logger = tfw::hal::logging::createLogger("SlavePort");
}

namespace tfw::hal::i2c {
    SlavePort::SlavePort(TwoWireType& twoWire) :
        selectedEndpoint(&endpoints[0]),
        twoWire(twoWire) {
        uint8_t i = 0;
        for (auto& endpoint : endpoints) {
            endpoint.id = i;
            i++;
        }
    }

    SlavePort::~SlavePort() = default;

    void SlavePort::setup(const uint8_t address, Pins pins, const uint8_t eventInterruptPin, const uint8_t discoveryPin) {
        twoWire.onReceive([this](const int len) { onReceiveCallback(len); });
        twoWire.onRequest([this]() { onRequestCallback(); });
        twoWire.begin(address, pins.SDA, pins.SCL, 100'000);

        eventInterruptOutputPin = gpio::OutputPin::physical(eventInterruptPin, OPEN_DRAIN);
        eventInterruptOutputPin->init();

        // Released until requestDiscovery()
        discoveryOutputPin = gpio::OutputPin::physical(discoveryPin, OPEN_DRAIN);
        discoveryOutputPin->init();
        discoveryOutputPin->setHigh();
    }

    void SlavePort::requestDiscovery() const {
        discoveryOutputPin->setLow();
    }

    void SlavePort::addCommandHandler(void* handler) {
        knownCommandsHandlers.push_back(handler);
    }

    void SlavePort::onReceiveCallback(const int len) {
        PendingCommand command;
        while (twoWire.available()) {
            const auto value = twoWire.read();
            if (command.length < sizeof(command.bytes)) {
                command.bytes[command.length++] = value;
            }
        }
        twoWire.flush();

        if (command.length == 0) {
            return;
        }

        // Executed right away, as the master reads the endpoint directly after selecting it. The selected endpoint
        // is only used by this task, as the receive and request callbacks run on the same task.
        if (command.bytes[0] == commands::builtin::SetEndpoint.id) {
            const auto message =
                reinterpret_cast<const commands::CommandMessage<commands::builtin::SetEndpointParams>*>(command.bytes);
            selectEndpoint(message->params.endpointId);
            return;
        }

        // Executed right away, as the master checks the discovery line again shortly after scanning
        if (command.bytes[0] == commands::builtin::AcknowledgeDiscovery.id) {
            discoveryOutputPin->setHigh();
            return;
        }

        std::lock_guard guard(lock);
        pendingCommands.enqueue(command);
    }

    void SlavePort::processPendingCommands() {
        while (true) {
            PendingCommand pendingCommand;
            {
                std::lock_guard guard(lock);

                const auto next = pendingCommands.dequeue();
                if (next == nullptr) {
                    return;
                }

                pendingCommand = *next;
            }

            for (auto& commandPtr : knownCommandsHandlers) {
                const auto command = static_cast<commands::RemoteCommandHandler<commands::AnyParams>*>(commandPtr);
                const auto message = command->parseMessage(pendingCommand.bytes);
                if (message->commandId != command->id) {
                    continue;
                }

                command->execute(message->targetDeviceId, &message->params); // TODO what to do with returned errors?
            }
        }
    }

    void SlavePort::onRequestCallback() {
        std::lock_guard guard(lock);

        if (selectedEndpoint->id == i2c::endpoints::builtin::Events.id) {
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

        twoWire.write(selectedEndpoint->data, selectedEndpoint->length);
    }

    void SlavePort::selectEndpoint(const uint8_t endpointId) {
        selectedEndpoint = &this->endpoints[endpointId];
    }

    void SlavePort::enqueueEvent(const Event& event) {
        std::lock_guard guard(lock);

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

#endif // ARDUINO
