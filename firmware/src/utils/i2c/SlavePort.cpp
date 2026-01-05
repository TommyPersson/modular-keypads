#include "SlavePort.h"

#include <cstring>

#include "Commands.h"
#include "Pins.h"
#include "utils/logging/Logger.h"

namespace {
    auto logger = utils::logging::createLogger("SlavePort");
}

namespace utils::i2c {
    SlavePort::SlavePort(TwoWire& twoWire) :
        selectedEndpoint(endpoints[0]),
        twoWire(twoWire),
        receiveArena(32) {
    }

    SlavePort::~SlavePort() = default;

    void SlavePort::setup(const uint8_t address, Pins pins) {
        twoWire.onReceive([this](const int len) { onReceiveCallback(len); });
        twoWire.onRequest([this]() { onRequestCallback(); });
        twoWire.begin(address, pins.SDA, pins.SCL, 100'000);

        addCommandHandler(new commands::LambdaCommandHandler<commands::builtin::SetEndpointParams>(
            commands::builtin::SetEndpoint.id,
            [this](const commands::builtin::SetEndpointParams& params) -> void_result {
                this->selectEndpoint(params.endpointId);
                return void_result::success();
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
            const auto command = static_cast<commands::CommandHandler<commands::AnyParams>*>(commandPtr);
            if (command->id != message[0]) {
                continue;
            }
            const auto params = command->parseData(message);
            command->execute(params);
        }

        receiveArena.reset();
    }

    void SlavePort::onRequestCallback() {
        std::lock_guard guard(lock);

        twoWire.write(selectedEndpoint.data, selectedEndpoint.length);
    }

    void SlavePort::selectEndpoint(const uint8_t endpointId) {
        selectedEndpoint = this->endpoints[endpointId];
    }
}
