#pragma once

#include "Firmware.h"

class MasterFirmware final : public Firmware {
public:
    explicit MasterFirmware(ServiceLocator& serviceLocator);
    ~MasterFirmware() override;

    void setup() override;
    void loop() override;
private:
};
