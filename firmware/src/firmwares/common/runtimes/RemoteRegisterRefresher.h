#pragma once

#include "RegisterRefresher.h"
#include "utils/i2c/Client.h"

class RemoteRegisterRefresher final : public RegisterRefresher {
public:
    RemoteRegisterRefresher(
        utils::registers::RegisterManager& registers,
        utils::i2c::Client& i2cClient,
        uint8_t deviceAddress,
        uint8_t numRegisters
    );

    void setup() override;
    void loop() override;

private:
    utils::i2c::Client& i2cClient;
    uint8_t deviceAddress;
    uint8_t numRegisters;
};
