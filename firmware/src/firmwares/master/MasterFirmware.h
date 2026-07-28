#pragma once

#include "firmwares/base/Firmware.h"
#include "TestModeController.h"
#include "KeyBindingSubSystem.h"
#include "../../mkp/devices/common/RemoteDevice.h"

class MasterFirmware final : public Firmware,
                             tfw::utils::observables::Observer<devices::DeviceSwitchEvent>,
                             tfw::utils::observables::Observer<devices::DeviceRotaryEncoderEvent> {
public:
    explicit MasterFirmware(ServiceLocator& serviceLocator);
    ~MasterFirmware() override;

    void setup() override;
    void loop() override;

    void observe(const devices::DeviceSwitchEvent& event) override;
    void observe(const devices::DeviceRotaryEncoderEvent& event) override;

private:
    void refreshConnectedDevices();

    TestModeController testModeController;

    std::unique_ptr<devices::LocalDevice> localDevice;
    std::vector<std::unique_ptr<devices::Device>> remoteDevices;
    std::vector<devices::Device*> allDevices;

    std::unique_ptr<common::macros::MacroStorage> macroStorage;
    std::unique_ptr<common::keybindings::KeyBindingStorage> keyBindingStorage;
    std::unique_ptr<KeyBindingSubSystem> keyBindingSubSystem;

    std::shared_ptr<tfw::hal::metrics::TimerMetric> loopTimerMetric;
};
