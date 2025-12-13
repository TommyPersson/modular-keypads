#pragma once

#include "firmwares/Firmware.h"

class GenericFirmware final : public Firmware {
public:
    explicit GenericFirmware(ServiceLocator& serviceLocator);

    ~GenericFirmware() override;

    void setup() override;
    void loop() override;
};
