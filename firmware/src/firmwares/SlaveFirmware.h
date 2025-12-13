#pragma once

#include "Firmware.h"

class SlaveFirmware final : public Firmware {
public:
    explicit SlaveFirmware(ServiceLocator& serviceLocator);
    ~SlaveFirmware() override;

    void setup() override;
    void loop() override;
private:
};
