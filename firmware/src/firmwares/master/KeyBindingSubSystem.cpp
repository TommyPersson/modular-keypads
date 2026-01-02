#include "KeyBindingSubSystem.h"

#include <bitset>

#include "utils/logging/Logger.h"

using namespace common::macros;
using namespace common::keybindings;

// TODO lots of allocation stuff to optimize

namespace {
    auto logger = utils::logging::createLogger("KeyBindingSubSystem");

    std::vector<uint8_t> getKeyCodesFromModifierFlags(const uint8_t modifiers) {
        std::vector<uint8_t> result(8);

        const std::bitset<8> flags(modifiers);

        if (flags.test(0)) result.push_back(0xe0); // Control (L)
        if (flags.test(1)) result.push_back(0xe1); // Shift (L)
        if (flags.test(2)) result.push_back(0xe2); // Alt (L)
        if (flags.test(3)) result.push_back(0xe3); // Meta (L)
        if (flags.test(4)) result.push_back(0xe4); // Control (R)
        if (flags.test(5)) result.push_back(0xe5); // Shift (R)
        if (flags.test(6)) result.push_back(0xe6); // Alt (R)
        if (flags.test(7)) result.push_back(0xe7); // Meta (R)

        return result;
    }

    std::vector<std::shared_ptr<utils::usb::Action>> getActionSequence(const ShortcutMacroData& data) {
        std::vector<std::shared_ptr<utils::usb::Action>> actions;

        std::vector<uint8_t> keyCodes(9);
        auto modifierKeyCodes = getKeyCodesFromModifierFlags(data.modifiers);
        keyCodes.insert(keyCodes.begin(), modifierKeyCodes.begin(), modifierKeyCodes.end());
        keyCodes.push_back(data.hidKeyCode);

        actions.push_back(utils::usb::Action::keyPress(keyCodes));

        return actions;
    }

    std::shared_ptr<CompiledMacro> compileMacro(const Macro& macro) {
        if (macro.data->type == SHORTCUT) {
            const auto& data = dynamic_cast<ShortcutMacroData&>(*macro.data);
            const auto sequence = getActionSequence(data);

            return std::make_shared<CompiledMacro>(CompiledMacro{
                .macroId = macro.data->id,
                .actions = sequence,
            });
        }

        if (macro.data->type == CONSUMER_CONTROL) {
            const auto& data = dynamic_cast<ConsumerControlMacroData&>(*macro.data);
            return std::make_shared<CompiledMacro>(CompiledMacro{
                .macroId = macro.data->id,
                .actions = std::vector{utils::usb::Action::consumerControl(data.usageId)},
            });
        }

        if (macro.data->type == SYSTEM_CONTROL) {
            const auto& data = dynamic_cast<SystemControlMacroData&>(*macro.data);
            return std::make_shared<CompiledMacro>(CompiledMacro{
                .macroId = macro.data->id,
                .actions = std::vector{utils::usb::Action::systemControl(data.code)},
            });
        }

        return nullptr;
    }
}

KeyBindingSubSystem::KeyBindingSubSystem(
    MacroStorage& macroStorage,
    KeyBindingStorage& keyBindingStorage,
    TestModeController& testModeController,
    utils::usb::Connection& usbConnection
) : macroStorage(macroStorage),
    keyBindingStorage(keyBindingStorage),
    testModeController(testModeController),
    usbConnection(usbConnection) {

    macroStorage.onMacroSaved().addObserver(this);
    macroStorage.onMacroRemoved().addObserver(this);
    keyBindingStorage.onKeyBindingSet().addObserver(this);
    keyBindingStorage.onKeyBindingCleared().addObserver(this);
}

KeyBindingSubSystem::~KeyBindingSubSystem() {
    macroStorage.onMacroSaved().removeObserver(this);
    macroStorage.onMacroRemoved().removeObserver(this);
    keyBindingStorage.onKeyBindingSet().removeObserver(this);
    keyBindingStorage.onKeyBindingCleared().removeObserver(this);
}

void KeyBindingSubSystem::setup() {
}

void KeyBindingSubSystem::loop() {
    if (macrosNeedRefresh) {
        refreshCompiledMacros();
        macrosNeedRefresh = false;
    }

    if (keyBindingsNeedRefresh) {
        refreshKeyBindings();
        keyBindingsNeedRefresh = false;
    }
}

void KeyBindingSubSystem::observe(const MacroSaved& event) {
    macrosNeedRefresh = true;
}

void KeyBindingSubSystem::observe(const MacroRemoved& event) {
    macrosNeedRefresh = true;
    keyBindingStorage.removeAll(event.macroId);
}

void KeyBindingSubSystem::observe(const KeyBindingSet& event) {
    keyBindingsNeedRefresh = true;
}

void KeyBindingSubSystem::observe(const KeyBindingCleared& event) {
    keyBindingsNeedRefresh = true;
}

void KeyBindingSubSystem::observe(const devices::DeviceSwitchEvent& event) {
    executeMacroFor(findKeyBinding(event));
}

void KeyBindingSubSystem::observe(const devices::DeviceRotaryEncoderEvent& event) {
    executeMacroFor(findKeyBinding(event));
}

std::shared_ptr<KeyBinding> KeyBindingSubSystem::findKeyBinding(const devices::DeviceSwitchEvent& event) {
    if (event.state != SwitchState::PRESSED) {
        return nullptr;
    }

    for (const auto& keyBinding : keyBindings) {
        if (keyBinding->trigger->type == PUSH_BUTTON) {
            const auto& trigger = dynamic_cast<PushButtonTrigger&>(*keyBinding->trigger);
            if (trigger.deviceId == event.deviceId && trigger.number == event.switchNumber) {
                return std::make_shared<KeyBinding>(*keyBinding);
            }
        }
    }

    return nullptr;
}

std::shared_ptr<KeyBinding> KeyBindingSubSystem::findKeyBinding(const devices::DeviceRotaryEncoderEvent& event) {
    for (const auto& keyBinding : keyBindings) {
        if (keyBinding->trigger->type == ROTARY_ENCODER) {
            const auto& trigger = dynamic_cast<RotaryEncoderTrigger&>(*keyBinding->trigger);
            if (trigger.deviceId == event.deviceId && trigger.number == event.encoderNumber && trigger.direction == event.direction) {
                return std::make_shared<KeyBinding>(*keyBinding);
            }
        }
    }

    return nullptr;
}


void KeyBindingSubSystem::executeMacroFor(const std::shared_ptr<KeyBinding>& keyBinding) {
    if (testModeController.isEnabled()) {
        return;
    }

    if (keyBinding == nullptr) {
        return;
    }

    for (const auto& macro : macros) {
        if (macro->macroId == keyBinding->macroId) {
            for (const auto& action : macro->actions) {
                usbConnection.sendAction(*action);
            }
        }
    }
}

void KeyBindingSubSystem::refreshCompiledMacros() {
    macros.clear();

    macroStorage.forEach([&](const Macro& macro) {
        auto compiledMacro = compileMacro(macro);
        if (compiledMacro != nullptr) {
            macros.push_back(compiledMacro);
        }
    });

    logger->info("Reloaded macros. # Found = %i", macros.size());
}

void KeyBindingSubSystem::refreshKeyBindings() {
    keyBindings.clear();

    // TODO large fixed arena for keybinding data?

    keyBindingStorage.forEach([&](const KeyBinding& keyBinding) {
        std::shared_ptr<Trigger> triggerCopy = nullptr;
        if (keyBinding.trigger->type == PUSH_BUTTON) {
            auto pushButtonTrigger = dynamic_cast<PushButtonTrigger&>(*keyBinding.trigger);
            triggerCopy = std::make_shared<PushButtonTrigger>(pushButtonTrigger);
        } else if (keyBinding.trigger->type == ROTARY_ENCODER) {
            auto rotaryEncoderTrigger = dynamic_cast<RotaryEncoderTrigger&>(*keyBinding.trigger);
            triggerCopy = std::make_shared<RotaryEncoderTrigger>(rotaryEncoderTrigger);
        }

        auto keyBindingCopy = std::make_shared<KeyBinding>(triggerCopy, keyBinding.macroId);

        keyBindings.push_back(keyBindingCopy);
    });

    logger->info("Reloaded key bindings. # Found = %i", keyBindings.size());
}