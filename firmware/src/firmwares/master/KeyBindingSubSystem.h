#pragma once

#include "TestModeController.h"
#include "firmwares/common/keybindings/KeyBindingStorage.h"
#include "firmwares/common/runtimes/DeviceRuntime.h"
#include "firmwares/common/macros/MacroStorage.h"
#include "firmwares/common/usb/USBConnection.h"

struct CompiledMacro {
    uint16_t macroId;
    std::vector<std::shared_ptr<usb::Action>> actions;
};

class KeyBindingSubSystem
    : Observer<common::macros::MacroSaved>,
      Observer<common::macros::MacroRemoved>,
      Observer<common::keybindings::KeyBindingSet>,
      Observer<common::keybindings::KeyBindingCleared>,
      Observer<devices::DeviceSwitchEvent>,
      Observer<devices::DeviceRotaryEncoderEvent> {
public:
    KeyBindingSubSystem(
        common::macros::MacroStorage& macroStorage,
        common::keybindings::KeyBindingStorage& keyBindingStorage,
        TestModeController& testModeController,
        usb::Connection& usbConnection
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
    usb::Connection& usbConnection;

    std::vector<std::shared_ptr<common::keybindings::KeyBinding>> keyBindings;
    std::vector<std::shared_ptr<CompiledMacro>> macros;
};
