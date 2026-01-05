#pragma once

#include <Wire.h>

#include "Endpoint.h"
#include "Commands.h"
#include "Pins.h"
#include "utils/result.h"

namespace utils::i2c {
    class Client {
    public:
        explicit Client(TwoWire& i2c) : i2c(i2c) {
        }

        ~Client() = default;

        void setup(const Pins& pins) {
            i2c.begin(pins.SDA, pins.SCL, 400'000);
        }

        template <class TParams>
        void_result sendCommand(const uint8_t address, const commands::CommandDescriptor<TParams>& command, const TParams& params) {
            uint8_t message[sizeof(TParams) + 1] = {};
            message[0] = command.id;
            memcpy(&message[1], &params, sizeof(TParams));

            i2c.beginTransmission(address);
            i2c.write(message, sizeof(message));
            const auto result = i2c.endTransmission();

            if (result == 0) {
                return void_result::success();
            }

            return void_result::error("unable.to.execute.command");
        }

        result<uint8_t*> readEndpointRaw(const uint8_t address, const uint8_t endpointId, const size_t size) {
            if (!setEndpoint(address, endpointId)) {
                // TODO cache last used endpoint for a device address?
                return result<uint8_t*>::error("unable.to.set.i2c.device.endpoint");
            }

            auto data = readCurrentEndpoint<uint8_t>(address, size);

            return result<uint8_t*>::of(data);
        }

        template <typename TStruct>
        result<TStruct*> readEndpoint(uint8_t address, const EndpointDescriptor<TStruct>& endpoint) {
            if (!setEndpoint(address, endpoint.id)) {
                // TODO cache last used endpoint for a device address?
                return result<TStruct*>::error("unable.to.set.i2c.device.endpoint");
            }

            auto data = readCurrentEndpoint<TStruct>(address, endpoint.length);

            return result<TStruct*>::of(data);
        }

    private:
        bool setEndpoint(uint8_t address, uint8_t endpointId) {
            auto result = sendCommand(address, commands::builtin::SetEndpoint, { .endpointId = endpointId });
            return !result.has_error;
        }

        template <typename TStruct>
        bool setEndpoint(uint8_t address, const EndpointDescriptor<TStruct>& endpoint) {
            return setEndpoint(address, endpoint.id);
        }

        template <typename TStruct>
        TStruct* readCurrentEndpoint(const uint8_t address, const size_t size = sizeof(TStruct)) {
            memset(readBuffer, 0, sizeof(readBuffer));

            i2c.requestFrom(address, size);

            int i = 0;
            while (i2c.available()) {
                int byte = i2c.read();
                if (i == 0 && byte == 0x17) {
                    // There is often an End of Transmission Block character on the bus?
                    continue;
                }
                readBuffer[i] = byte;
                i++;
            }

            auto data = reinterpret_cast<TStruct*>(&readBuffer);
            return data;
        }

        TwoWire& i2c;
        uint8_t readBuffer[32]{};
    };
}
