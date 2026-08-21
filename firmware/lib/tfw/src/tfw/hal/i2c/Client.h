#pragma once

#include <Wire.h>

#include <tfw/utils/result.h>
#include "Commands.h"
#include "Endpoint.h"
#include "Pins.h"

namespace tfw::hal::i2c {
    class Client {
    public:
        explicit Client(TwoWire& i2c) : i2c(i2c) {
        }

        ~Client() = default;

        void setup(const Pins& pins) {
            i2c.begin(pins.SDA, pins.SCL, 400'000);
        }

        template <class TParams>
        utils::void_result sendCommand(
            const uint8_t address,
            const uint64_t deviceId,
            const commands::RemoteCommandDescriptor<TParams>& command,
            const TParams& params
        ) {
            commands::CommandMessage<TParams> message{
                .commandId = command.id,
                .targetDeviceId = deviceId,
                .params = params
            };

            const auto bytes = reinterpret_cast<uint8_t*>(&message);

            i2c.beginTransmission(address);
            i2c.write(bytes, sizeof(message));
            const auto result = i2c.endTransmission();
            i2c.flush();

            if (result == 0) {
                return utils::void_result::success();
            }

            return utils::void_result::error("unable.to.execute.remote.command");
        }

        utils::result<uint8_t*> readEndpointRaw(const uint8_t address, const uint8_t endpointId, const size_t size) {
            if (!setEndpoint(address, endpointId)) {
                // TODO cache last used endpoint for a device address?
                return utils::result<uint8_t*>::error("unable.to.set.i2c.device.endpoint");
            }

            auto data = readCurrentEndpoint<uint8_t>(address, size);

            return utils::result<uint8_t*>::of(data);
        }

        template <typename TStruct>
        utils::result<TStruct*> readEndpoint(uint8_t address, const EndpointDescriptor<TStruct>& endpoint) {
            if (!setEndpoint(address, endpoint.id)) {
                // TODO cache last used endpoint for a device address?
                return utils::result<TStruct*>::error("unable.to.set.i2c.device.endpoint");
            }

            auto data = readCurrentEndpoint<TStruct>(address, endpoint.length);

            return utils::result<TStruct*>::of(data);
        }

    private:
        bool setEndpoint(uint8_t address, uint8_t endpointId) {
            auto result = sendCommand(address, 0, commands::builtin::SetEndpoint, {.endpointId = endpointId});
            return !result.has_error;
        }

        template <typename TStruct>
        bool setEndpoint(uint8_t address, const EndpointDescriptor<TStruct>& endpoint) {
            return setEndpoint(address, endpoint.id);
        }

        template <typename TStruct>
        TStruct* readCurrentEndpoint(const uint8_t address, const size_t size = sizeof(TStruct)) {
            memset(readBuffer, 0, sizeof(readBuffer));

            i2c.flush();
            i2c.requestFrom(address, size);

            int i = 0;
            while (i2c.available()) {
                int byte = i2c.read();
                readBuffer[i] = byte;
                i++;
            }
            i2c.flush();

            auto data = reinterpret_cast<TStruct*>(&readBuffer);
            return data;
        }

        TwoWire& i2c;
        uint8_t readBuffer[32]{};
    };
}
