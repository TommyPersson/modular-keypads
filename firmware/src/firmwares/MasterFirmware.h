#pragma once

#include "Firmware.h"

class MasterFirmware final : public Firmware,
                             Observer<devices::DeviceSwitchEvent> {
public:
    explicit MasterFirmware(ServiceLocator& serviceLocator);
    ~MasterFirmware() override;

    void setup() override;
    void loop() override;

private:
    void observe(const devices::DeviceSwitchEvent& event) override;

private:
    std::vector<std::unique_ptr<devices::DeviceModule>> devices;
};
