#pragma once

#include "firmwares/Firmware.h"

class GenericFirmware final : public Firmware {
public:
    explicit GenericFirmware(
        DeviceConfigurationManager& deviceConfigurationManager,
        SerialPort& serialPort,
        Notifier& notifier,
        Logger& logger,
        TwoWire& i2c
    );

    ~GenericFirmware() override;

    void setup() override;
    void loop() override;
};
