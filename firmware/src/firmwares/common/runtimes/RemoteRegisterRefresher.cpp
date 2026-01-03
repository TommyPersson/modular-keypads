#include "RemoteRegisterRefresher.h"

#include <utils/i2c/EndpointStructs.h>

RemoteRegisterRefresher::RemoteRegisterRefresher(
    utils::registers::RegisterManager& registers,
    utils::i2c::Client& i2cClient,
    uint8_t deviceAddress
    ) : i2cClient(i2cClient), deviceAddress(deviceAddress), RegisterRefresher(registers) {

}

void RemoteRegisterRefresher::begin() {
}

void RemoteRegisterRefresher::loop() {
    if (!i2cClient.setEndpoint(deviceAddress, utils::i2c::Endpoint::Registers)) {
        return;
    }

    auto response = i2cClient.readEndpoint<utils::i2c::structs::DeviceRegisters>(deviceAddress);
    auto data = std::span<uint8_t, 30>(response->data, response->data + sizeof(response->data));

    registers.writeAll(data);

}