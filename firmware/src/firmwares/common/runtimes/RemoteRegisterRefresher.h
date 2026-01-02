#pragma once

#include <firmwares/common/i2c/I2cClient.h>

#include "RegisterRefresher.h"

class RemoteRegisterRefresher final : public RegisterRefresher {
public:
    RemoteRegisterRefresher(
        utils::registers::RegisterManager& registers,
        I2cClient& i2cClient,
        uint8_t deviceAddress
    );

    void begin() override;
    void loop() override;

private:
    I2cClient& i2cClient;
    uint8_t deviceAddress;
};
