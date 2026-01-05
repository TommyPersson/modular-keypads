#include "RemoteRegisterRefresher.h"

#include <firmwares/modules/common/DeviceModule.h>

RemoteRegisterRefresher::RemoteRegisterRefresher(
    utils::registers::RegisterManager& registers,
    utils::i2c::Client& i2cClient,
    uint8_t deviceAddress,
    uint8_t numRegisters
) : RegisterRefresher(registers),
    i2cClient(i2cClient),
    deviceAddress(deviceAddress),
    numRegisters(numRegisters) {
}

void RemoteRegisterRefresher::setup() {
}

void RemoteRegisterRefresher::loop() {
    auto result = i2cClient.readEndpointRaw(deviceAddress, devices::common::i2c::endpoints::DeviceRegisters.id, numRegisters);
    if (result.has_error) {
        return;
    }

    auto registerData = result.value;

    auto data = std::span<uint8_t, 30>(registerData, registerData + numRegisters);

    registers.writeAll(data);
}
