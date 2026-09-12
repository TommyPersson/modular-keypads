#pragma once

#include "mkp/firmwares/base/Firmware.h"
#include "mkp/components/keybindings/KeyBindingExecutor.h"
#include "mkp/components/keybindings/TestModeController.h"

class MasterFirmware final
    : public mkp::firmwares::base::Firmware,
      tfw::utils::observables::Observer<mkp::devices::common::DeviceSwitchEvent>,
      tfw::utils::observables::Observer<mkp::devices::common::DeviceRotaryEncoderEvent>,
      tfw::utils::observables::Observer<tfw::hal::gpio::InputPinInterruptEvent> {
public:
    explicit MasterFirmware(mkp::firmwares::base::ServiceLocator& serviceLocator);
    ~MasterFirmware() override;

    void setup() override;
    void loop() override;

    void observe(const mkp::devices::common::DeviceSwitchEvent& event) override;
    void observe(const mkp::devices::common::DeviceRotaryEncoderEvent& event) override;
    void observe(const tfw::hal::gpio::InputPinInterruptEvent& event) override;

private:
    void refreshRemoteDevices();

private:
    TestModeController testModeController;

    std::unique_ptr<mkp::devices::common::LocalDevice> localDevice;
    std::vector<std::unique_ptr<mkp::devices::common::RemoteDevice>> remoteDevices;
    std::vector<mkp::devices::common::Device*> allDevices;

    std::unique_ptr<mkp::components::macros::MacroStorage> macroStorage;
    std::unique_ptr<mkp::components::keybindings::KeyBindingStorage> keyBindingStorage;
    std::unique_ptr<mkp::components::keybindings::KeyBindingExecutor> keyBindingExecutor;

    std::shared_ptr<tfw::hal::metrics::TimerMetric> loopTimerMetric;
    std::unique_ptr<tfw::hal::gpio::InputPin> slaveEventInterruptInputPin;
};
