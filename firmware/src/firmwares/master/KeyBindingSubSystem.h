#pragma once

#include "TestModeController.h"
#include "firmwares/common/keybindings/KeyBindingStorage.h"
#include "firmwares/common/runtimes/DeviceRuntime.h"
#include "firmwares/common/macros/MacroStorage.h"
#include "utils/usb/USBConnection.h"

struct CompiledMacro {
    uint16_t macroId;
    std::vector<std::shared_ptr<utils::usb::Action>> actions;
};

class KeyBindingSubSystem
    : utils::observables::Observer<common::macros::MacroSaved>,
      utils::observables::Observer<common::macros::MacroRemoved>,
      utils::observables::Observer<common::keybindings::KeyBindingSet>,
      utils::observables::Observer<common::keybindings::KeyBindingCleared>,
      utils::observables::Observer<devices::DeviceSwitchEvent>,
      utils::observables::Observer<devices::DeviceRotaryEncoderEvent> {
public:
    KeyBindingSubSystem(
        common::macros::MacroStorage& macroStorage,
        common::keybindings::KeyBindingStorage& keyBindingStorage,
        TestModeController& testModeController,
        utils::usb::Connection& usbConnection
    );
    ~KeyBindingSubSystem() override;

    void setup();
    void loop();

    void observe(const common::macros::MacroSaved& event) override;
    void observe(const common::macros::MacroRemoved& event) override;
    void observe(const common::keybindings::KeyBindingSet& event) override;
    void observe(const common::keybindings::KeyBindingCleared& event) override;
    void observe(const devices::DeviceSwitchEvent& event) override;
    void observe(const devices::DeviceRotaryEncoderEvent& event) override;

private:
    void refreshCompiledMacros();
    void refreshKeyBindings();

    std::shared_ptr<common::keybindings::KeyBinding> findKeyBinding(const devices::DeviceSwitchEvent& event);
    std::shared_ptr<common::keybindings::KeyBinding> findKeyBinding(const devices::DeviceRotaryEncoderEvent& event);

    void executeMacroFor(const std::shared_ptr<common::keybindings::KeyBinding>& keyBinding);

    bool macrosNeedRefresh = true;
    bool keyBindingsNeedRefresh = true;

    common::macros::MacroStorage& macroStorage;
    common::keybindings::KeyBindingStorage& keyBindingStorage;
    TestModeController& testModeController;
    utils::usb::Connection& usbConnection;

    std::vector<std::shared_ptr<common::keybindings::KeyBinding>> keyBindings;
    std::vector<std::shared_ptr<CompiledMacro>> macros;
};
