#include "SlavePort.h"

#ifdef ARDUINO

#include <cstring>
#include <esp_random.h>
#include <Wire.h>

#include <tfw/hal/logging.h>
#include "AddressAllocator.h"
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

    void SlavePort::setup(const uint64_t deviceId, Pins pins, const uint8_t eventInterruptPin, const uint8_t discoveryPin) {
        this->deviceId = deviceId;
        this->pins = pins;

        const auto identity = Identity::of(deviceId);
        updateEndpoint(endpoints::builtin::Identity, &identity);

        twoWire.onReceive([this](const int len) { onReceiveCallback(len); });
        twoWire.onRequest([this]() { onRequestCallback(); });
        listenAt(randomTemporaryAddress());

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

            if (processAddressCommand(pendingCommand)) {
                continue;
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

    bool SlavePort::processAddressCommand(const PendingCommand& command) {
        const auto commandId = command.bytes[0];

        if (commandId == commands::builtin::AssignAddress.id) {
            const auto message =
                reinterpret_cast<const commands::CommandMessage<commands::builtin::AssignAddressParams>*>(command.bytes);

            // Other devices at the same temporary address receive the command too
            if (message->targetDeviceId == deviceId) {
                listenAt(message->params.address);
            }

            return true;
        }

        if (commandId == commands::builtin::RerollAddress.id) {
            auto address = randomTemporaryAddress();
            while (address == currentAddress) {
                address = randomTemporaryAddress();
            }

            listenAt(address);
            return true;
        }

        return false;
    }

    // Not called with the lock held, as the receive and request callbacks take it on the Wire task
    void SlavePort::listenAt(const uint8_t address) {
        twoWire.end();
        if (!twoWire.begin(address, pins.SDA, pins.SCL, 100'000)) {
            logger->error("Unable to listen at address 0x%02x", address);
            return;
        }

        currentAddress = address;
        logger->info("Listening at address 0x%02x", address);
    }

    uint8_t SlavePort::randomTemporaryAddress() {
        constexpr auto count = addresses::TEMPORARY_LAST - addresses::TEMPORARY_FIRST + 1;
        return addresses::TEMPORARY_FIRST + esp_random() % count;
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
