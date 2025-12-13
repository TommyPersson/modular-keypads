#pragma once

#include <Wire.h>

#include "Endpoint.h"
#include "Operation.h"


class I2cClient {
public:
    explicit I2cClient(TwoWire& i2c) : i2c(i2c) {}
    ~I2cClient() {}

    bool setEndpoint(uint8_t address, i2c::Endpoint endpoint) {
        i2c.beginTransmission(address);
        const uint8_t message[] = {(uint8_t)i2c::Operation::SetEndpoint, (uint8_t)endpoint};
        i2c.write(message, sizeof(message));
        const auto result = i2c.endTransmission();
        return result == 0;
    }

    template <typename T>
    T* readEndpoint(const uint8_t address, const size_t size = sizeof(T)) {
        memset(readBuffer, 0, sizeof(readBuffer));

        i2c.requestFrom(address, size);

        int i = 0;
        while (i2c.available()) {
            readBuffer[i] = i2c.read();
            i++;
        }

        auto deviceInformation = reinterpret_cast<T*>(&readBuffer);
        return deviceInformation;
    }

private:
    TwoWire& i2c;
    uint8_t readBuffer[32];
};
