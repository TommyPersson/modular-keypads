#pragma once

#include "RegisterRefresher.h"
#include <tfw/hal/i2c.h>

class RemoteRegisterRefresher final : public RegisterRefresher {
public:
    RemoteRegisterRefresher(
        tfw::utils::registers::RegisterManager& registers,
        tfw::utils::i2c::Client& i2cClient,
        uint8_t deviceAddress,
        uint8_t numRegisters
    );

    void setup() override;
    void loop() override;

private:
    tfw::utils::i2c::Client& i2cClient;
    uint8_t deviceAddress;
    uint8_t numRegisters;
};
