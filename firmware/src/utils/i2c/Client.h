#pragma once

#include <Wire.h>
#include <utils/result.h>

#include "Endpoint.h"
#include "Pins.h"
#include "Operation.h"

namespace utils::i2c {
class Client {
public:
    explicit Client(TwoWire& i2c) : i2c(i2c) {}
    ~Client() = default;

    void setup(const Pins& pins) {
        i2c.begin(pins.SDA, pins.SCL, 400000);
    }

    template <typename TStruct>
    result<TStruct*> readEndpoint(uint8_t address, const EndpointDescriptor<TStruct>& endpoint) {
        if (!setEndpoint(address, endpoint)) { // TODO cache last used endpoint for a device address?
            return result<TStruct*>::error("unable.to.set.i2c.device.endpoint");
        }

        auto data = readEndpoint<TStruct>(address, sizeof(TStruct));

        return result<TStruct*>::of(data);
    }

private:
    template <typename TStruct>
    bool setEndpoint(uint8_t address, const EndpointDescriptor<TStruct>& endpoint) {
        i2c.beginTransmission(address);
        const uint8_t message[] = {(uint8_t)Operation::SetEndpoint, endpoint.id};
        i2c.write(message, sizeof(message));
        const auto result = i2c.endTransmission();
        return result == 0;
    }

    template <typename TStruct>
    TStruct* readEndpoint(const uint8_t address, const size_t size = sizeof(TStruct)) {
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
