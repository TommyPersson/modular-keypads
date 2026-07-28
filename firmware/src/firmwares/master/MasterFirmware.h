#pragma once

#include "firmwares/base/Firmware.h"
#include "TestModeController.h"
#include "KeyBindingSubSystem.h"
#include "../../mkp/devices/common/RemoteDevice.h"

class MasterFirmware final : public Firmware,
                             tfw::utils::observables::Observer<mkp::devices::common::DeviceSwitchEvent>,
                             tfw::utils::observables::Observer<mkp::devices::common::DeviceRotaryEncoderEvent> {
public:
    explicit MasterFirmware(ServiceLocator& serviceLocator);
    ~MasterFirmware() override;

    void setup() override;
    void loop() override;

    void observe(const mkp::devices::common::DeviceSwitchEvent& event) override;
    void observe(const mkp::devices::common::DeviceRotaryEncoderEvent& event) override;

private:
    void refreshRemoteDevices();

    TestModeController testModeController;

    std::unique_ptr<mkp::devices::common::LocalDevice> localDevice;
    std::vector<std::unique_ptr<mkp::devices::common::Device>> remoteDevices;
    std::vector<mkp::devices::common::Device*> allDevices;

    std::unique_ptr<common::macros::MacroStorage> macroStorage;
    std::unique_ptr<common::keybindings::KeyBindingStorage> keyBindingStorage;
    std::unique_ptr<KeyBindingSubSystem> keyBindingSubSystem;

    std::shared_ptr<tfw::hal::metrics::TimerMetric> loopTimerMetric;
};
