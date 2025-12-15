#include "RemoteRegisterRefresher.h"

#include <firmwares/common/i2c/EndpointStructs.h>

RemoteRegisterRefresher::RemoteRegisterRefresher(
    RegisterManager& registers,
    I2cClient& i2cClient,
    uint8_t deviceAddress
    ) : i2cClient(i2cClient), deviceAddress(deviceAddress), RegisterRefresher(registers) {

}

void RemoteRegisterRefresher::begin() {
}

void RemoteRegisterRefresher::loop() {
    if (!i2cClient.setEndpoint(deviceAddress, i2c::Endpoint::Registers)) {
        return;
    }

    auto response = i2cClient.readEndpoint<i2c::structs::DeviceRegisters>(deviceAddress);
    auto data = std::span<uint8_t, 32>(response->data, response->data + 32);

    registers.writeAll(data);

}