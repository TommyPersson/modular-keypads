#pragma once

#include "../base/Firmware.h"

class MasterFirmware final : public Firmware,
                             Observer<devices::DeviceSwitchEvent> {
public:
    explicit MasterFirmware(ServiceLocator& serviceLocator);
    ~MasterFirmware() override;

    void setup() override;
    void loop() override;

private:
    void refreshConnectedDevices();
    void observe(const devices::DeviceSwitchEvent& event) override;

    std::unique_ptr<devices::DeviceModule> localDevice;
    std::vector<std::unique_ptr<devices::DeviceModule>> connectedDevices;
};
