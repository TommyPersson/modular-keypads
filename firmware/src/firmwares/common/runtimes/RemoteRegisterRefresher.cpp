#include "RemoteRegisterRefresher.h"

#include <firmwares/modules/common/DeviceModule.h>

RemoteRegisterRefresher::RemoteRegisterRefresher(
    utils::registers::RegisterManager& registers,
    utils::i2c::Client& i2cClient,
    uint8_t deviceAddress
    ) : i2cClient(i2cClient), deviceAddress(deviceAddress), RegisterRefresher(registers) {

}

void RemoteRegisterRefresher::begin() {
}

void RemoteRegisterRefresher::loop() {
    if (!i2cClient.setEndpoint(deviceAddress, devices::common::i2c::endpoints::DeviceRegisters)) {
        return;
    }

    auto response = i2cClient.readEndpoint<devices::common::i2c::structs::DeviceRegisters>(deviceAddress);
    auto data = std::span<uint8_t, 30>(response->data, response->data + sizeof(response->data));

    registers.writeAll(data);

}