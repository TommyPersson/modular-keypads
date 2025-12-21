#pragma once

#include "firmwares/base/Firmware.h"

class SlaveFirmware final : public Firmware {
public:
    explicit SlaveFirmware(ServiceLocator& serviceLocator);
    ~SlaveFirmware() override;

    void setup() override;
    void loop() override;

private:
    std::unique_ptr<devices::DeviceModule> device;

    i2c::SlavePort& slavePort;
};
