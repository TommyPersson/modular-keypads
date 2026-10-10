#pragma once

#include "mkp/firmwares/base/Firmware.h"
#include "mkp/components/keybindings/KeyBindingExecutor.h"
#include "mkp/components/keybindings/TestModeController.h"

class MasterFirmware final
    : public mkp::firmwares::base::Firmware,
      tfw::utils::observables::Observer<mkp::devices::common::DeviceConnectedEvent>,
      tfw::utils::observables::Observer<mkp::devices::common::DeviceDisconnectedEvent>,
      tfw::utils::observables::Observer<mkp::devices::common::DeviceRenamedEvent>,
      tfw::utils::observables::Observer<mkp::devices::common::DeviceSwitchEvent>,
      tfw::utils::observables::Observer<mkp::devices::common::DeviceRotaryEncoderEvent>,
      tfw::utils::observables::Observer<tfw::hal::gpio::InputPinInterruptEvent> {
public:
    explicit MasterFirmware(mkp::firmwares::base::ServiceLocator& serviceLocator);
    ~MasterFirmware() override;

    void setup() override;
    void loop() override;

    void observe(const mkp::devices::common::DeviceConnectedEvent& event) override;
    void observe(const mkp::devices::common::DeviceDisconnectedEvent& event) override;
    void observe(const mkp::devices::common::DeviceRenamedEvent& event) override;
    void observe(const mkp::devices::common::DeviceSwitchEvent& event) override;
    void observe(const mkp::devices::common::DeviceRotaryEncoderEvent& event) override;
    void observe(const tfw::hal::gpio::InputPinInterruptEvent& event) override;

private:
    void refreshRemoteDevices();
    void rebuildAllDevices();
    void observeDevice(mkp::devices::common::Device& device);

private:
    TestModeController testModeController;

    std::unique_ptr<mkp::devices::common::LocalDevice> localDevice;
    std::vector<std::unique_ptr<mkp::devices::common::RemoteDevice>> remoteDevices;
    std::vector<mkp::devices::common::Device*> allDevices;

    // Scans are run from the loop, as they can replace devices that are notifying, or are being iterated over
    bool remoteDeviceScanRequested = false;

    // Slaves pull the shared discovery line low until they're scanned and acknowledged
    std::unique_ptr<tfw::hal::gpio::InputPin> discoveryInputPin;
    std::unique_ptr<tfw::hal::time::CountDown> discoveryScanCountDown;

    // Remote devices are checked periodically, as unplugged devices go unnoticed otherwise
    std::unique_ptr<tfw::hal::time::Timer> presenceCheckTimer;

    std::unique_ptr<mkp::components::macros::MacroStorage> macroStorage;
    std::unique_ptr<mkp::components::keybindings::KeyBindingStorage> keyBindingStorage;
    std::unique_ptr<mkp::components::keybindings::KeyBindingExecutor> keyBindingExecutor;

    std::shared_ptr<tfw::hal::metrics::TimerMetric> loopTimerMetric;
    std::unique_ptr<tfw::hal::gpio::InputPin> slaveEventInterruptInputPin;

    std::unique_ptr<mkp::components::notifications::Notifier> notifier;
};
