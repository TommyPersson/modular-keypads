#include "RemoteRegisterRefresher.h"

#include <firmwares/modules/common/DeviceModule.h>

RemoteRegisterRefresher::RemoteRegisterRefresher(
    utils::registers::RegisterManager& registers,
    utils::i2c::Client& i2cClient,
    uint8_t deviceAddress
) : RegisterRefresher(registers), i2cClient(i2cClient), deviceAddress(deviceAddress) {
}

void RemoteRegisterRefresher::begin() {
}

void RemoteRegisterRefresher::loop() {
    auto result = i2cClient.readEndpoint(deviceAddress, devices::common::i2c::endpoints::DeviceRegisters);
    if (result.has_error) {
        return;
    }

    auto deviceRegisters = result.value;

    auto data = std::span<uint8_t, 30>(deviceRegisters->data, deviceRegisters->data + sizeof(deviceRegisters->data));

    registers.writeAll(data);
}
