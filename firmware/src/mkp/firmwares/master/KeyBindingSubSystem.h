#pragma once

#include <tfw/hal/metrics.h>
#include <tfw/hal/usb.h>

#include "TestModeController.h"
#include "mkp/components/macros/MacroStorage.h"
#include "mkp/components/keybindings/KeyBindingStorage.h"
#include "mkp/devices/common/Device.h"

struct CompiledMacro {
    uint16_t macroId;
    std::vector<std::shared_ptr<tfw::hal::usb::Action>> actions;
};

class KeyBindingSubSystem
    : tfw::utils::observables::Observer<mkp::components::macros::MacroSaved>,
      tfw::utils::observables::Observer<mkp::components::macros::MacroRemoved>,
      tfw::utils::observables::Observer<mkp::components::keybindings::KeyBindingSet>,
      tfw::utils::observables::Observer<mkp::components::keybindings::KeyBindingCleared>,
      tfw::utils::observables::Observer<mkp::devices::common::DeviceSwitchEvent>,
      tfw::utils::observables::Observer<mkp::devices::common::DeviceRotaryEncoderEvent> {
public:
    KeyBindingSubSystem(
        mkp::components::macros::MacroStorage& macroStorage,
        mkp::components::keybindings::KeyBindingStorage& keyBindingStorage,
        TestModeController& testModeController,
        tfw::hal::usb::Connection& usbConnection,
        tfw::hal::metrics::MetricRegistry& metricRegistry
    );
    ~KeyBindingSubSystem() override;

    void setup();
    void loop();

    uint32_t getNumAssigned() const { return keyBindings.size(); }

    void observe(const mkp::components::macros::MacroSaved& event) override;
    void observe(const mkp::components::macros::MacroRemoved& event) override;
    void observe(const mkp::components::keybindings::KeyBindingSet& event) override;
    void observe(const mkp::components::keybindings::KeyBindingCleared& event) override;
    void observe(const mkp::devices::common::DeviceSwitchEvent& event) override;
    void observe(const mkp::devices::common::DeviceRotaryEncoderEvent& event) override;

private:
    void refreshCompiledMacros();
    void refreshKeyBindings();

    std::shared_ptr<mkp::components::keybindings::KeyBinding> findKeyBinding(const mkp::devices::common::DeviceSwitchEvent& event);
    std::shared_ptr<mkp::components::keybindings::KeyBinding> findKeyBinding(const mkp::devices::common::DeviceRotaryEncoderEvent& event);

    void executeMacroFor(const std::shared_ptr<mkp::components::keybindings::KeyBinding>& keyBinding);

    bool macrosNeedRefresh = true;
    bool keyBindingsNeedRefresh = true;

    mkp::components::macros::MacroStorage& macroStorage;
    mkp::components::keybindings::KeyBindingStorage& keyBindingStorage;
    TestModeController& testModeController;
    tfw::hal::usb::Connection& usbConnection;

    std::vector<std::shared_ptr<mkp::components::keybindings::KeyBinding>> keyBindings;
    std::vector<std::shared_ptr<CompiledMacro>> macros;
};
