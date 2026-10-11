#include "KeyBindingExecutor.h"

#include <tfw/utils/strings.h>

#include <tfw/hal/logging.h>

#include "mkp/components/settings/Settings.h"

using namespace mkp::components::macros;
using namespace mkp::components::keybindings;
using namespace mkp::devices::common;
using tfw::hal::buttons::ButtonState;

// TODO lots of allocation stuff to optimize

namespace {
    auto logger = tfw::hal::logging::createLogger("KeyBindingSubSystem");

    std::shared_ptr<CompiledMacro> compileMacro(const Macro& macro) {
        if (macro.data->type == SHORTCUT) {
            const auto& data = dynamic_cast<ShortcutMacroData&>(*macro.data);
            const auto action = tfw::hal::usb::Action::shortcut(
                data.modifiers,
                data.hidKeyCode,
                data.holdModifiers,
                data.holdTimeMs
            );
            return std::make_shared<CompiledMacro>(CompiledMacro{
                .macroId = macro.data->id,
                .actions = std::vector{action},
            });
        }

        if (macro.data->type == CONSUMER_CONTROL) {
            const auto& data = dynamic_cast<ConsumerControlMacroData&>(*macro.data);
            return std::make_shared<CompiledMacro>(CompiledMacro{
                .macroId = macro.data->id,
                .actions = std::vector{tfw::hal::usb::Action::consumerControl(data.usageId)},
            });
        }

        if (macro.data->type == SYSTEM_CONTROL) {
            const auto& data = dynamic_cast<SystemControlMacroData&>(*macro.data);
            return std::make_shared<CompiledMacro>(CompiledMacro{
                .macroId = macro.data->id,
                .actions = std::vector{tfw::hal::usb::Action::systemControl(data.code)},
            });
        }

        if (macro.data->type == TEXT) {
            const auto& data = dynamic_cast<TextMacroData&>(*macro.data);
            const auto encodedText = data.text;
            char decodedText[encodedText.length() + 1];
            tfw::utils::strings::uriDecode(encodedText, decodedText, encodedText.length() + 1);

            return std::make_shared<CompiledMacro>(CompiledMacro{
                .macroId = macro.data->id,
                .actions = std::vector{tfw::hal::usb::Action::type(decodedText)},
            });
        }

        if (macro.data->type == MOUSE_SCROLL) {
            const auto& data = dynamic_cast<MouseScrollMacroData&>(*macro.data);
            return std::make_shared<CompiledMacro>(CompiledMacro{
                .macroId = macro.data->id,
                .actions = std::vector{tfw::hal::usb::Action::mouse(data.modifiers, 0, 0, 0, data.wheel, data.pan)},
            });
        }

        if (macro.data->type == MOUSE_BUTTON) {
            const auto& data = dynamic_cast<MouseButtonMacroData&>(*macro.data);
            return std::make_shared<CompiledMacro>(CompiledMacro{
                .macroId = macro.data->id,
                .actions = std::vector{tfw::hal::usb::Action::mouse(data.modifiers, data.buttons, 0, 0, 0, 0)},
            });
        }

        if (macro.data->type == MOUSE_MOVEMENT) {
            const auto& data = dynamic_cast<MouseMovementMacroData&>(*macro.data);
            return std::make_shared<CompiledMacro>(CompiledMacro{
                .macroId = macro.data->id,
                .actions = std::vector{tfw::hal::usb::Action::mouse(data.modifiers, 0, data.x, data.y, 0, 0)},
            });
        }

        if (macro.data->type == SEQUENCE) {
            const auto& data = dynamic_cast<SequenceMacroData&>(*macro.data);
            return std::make_shared<CompiledMacro>(CompiledMacro{
                .macroId = macro.data->id,
                .actions = {},
                .steps = data.steps,
            });
        }
        return nullptr;
    }
}

KeyBindingExecutor::KeyBindingExecutor(
    MacroStorage& macroStorage,
    KeyBindingStorage& keyBindingStorage,
    TestModeController& testModeController,
    tfw::hal::usb::Connection& usbConnection,
    tfw::hal::settings::DeviceSettings& deviceSettings,
    tfw::hal::metrics::MetricRegistry& metricRegistry,
    const tfw::hal::time::Clock& clock
) : macroStorage(macroStorage),
    keyBindingStorage(keyBindingStorage),
    testModeController(testModeController),
    usbConnection(usbConnection),
    deviceSettings(deviceSettings),
    sequenceRunner(
        clock,
        [this] { return this->usbConnection.isBusy(); },
        [this](const uint16_t macroId) { executeSequenceStep(macroId); }
    ) {

    macroStorage.onMacroSaved().addObserver(this);
    macroStorage.onMacroRemoved().addObserver(this);
    keyBindingStorage.onKeyBindingSet().addObserver(this);
    keyBindingStorage.onKeyBindingCleared().addObserver(this);

    metricRegistry.add(tfw::hal::metrics::lambda_gauge("macros.num_stored", [this] {
        return this->macroStorage.getNumStored();
    }));

    metricRegistry.add(tfw::hal::metrics::lambda_gauge("keybindings.num_assigned", [this] {
        return this->getNumAssigned();
    }));
}

KeyBindingExecutor::~KeyBindingExecutor() {
    macroStorage.onMacroSaved().removeObserver(this);
    macroStorage.onMacroRemoved().removeObserver(this);
    keyBindingStorage.onKeyBindingSet().removeObserver(this);
    keyBindingStorage.onKeyBindingCleared().removeObserver(this);
}

void KeyBindingExecutor::setup() {
}

void KeyBindingExecutor::loop() {
    if (macrosNeedRefresh) {
        refreshCompiledMacros();
        macrosNeedRefresh = false;
    }

    if (keyBindingsNeedRefresh) {
        refreshKeyBindings();
        keyBindingsNeedRefresh = false;
    }

    // Sequences stop when test mode is enabled, as their keys would no longer send anything either
    if (testModeController.isEnabled()) {
        sequenceRunner.stopAll();
    } else {
        sequenceRunner.update();
    }
}

void KeyBindingExecutor::observe(const MacroSaved& event) {
    macrosNeedRefresh = true;
}

void KeyBindingExecutor::observe(const MacroRemoved& event) {
    macrosNeedRefresh = true;
    keyBindingStorage.removeAll(event.macroId);
}

void KeyBindingExecutor::observe(const KeyBindingSet& event) {
    keyBindingsNeedRefresh = true;
}

void KeyBindingExecutor::observe(const KeyBindingCleared& event) {
    keyBindingsNeedRefresh = true;
}

void KeyBindingExecutor::observe(const DeviceSwitchEvent& event) {
    const auto button = std::make_pair(event.deviceId, event.switchNumber);
    const auto pressKeyBinding = findKeyBinding(event.deviceId, event.switchNumber, ButtonState::PRESSED);
    const auto longPressKeyBinding = findKeyBinding(event.deviceId, event.switchNumber, ButtonState::LONG_PRESSED);

    switch (event.state) {
    case ButtonState::PRESSED:
        if (longPressKeyBinding == nullptr) {
            executeMacroFor(pressKeyBinding);
        } else {
            // Not known yet whether it's a press or a long press
            pendingPresses.insert(button);
        }
        break;
    case ButtonState::LONG_PRESSED:
        pendingPresses.erase(button);
        executeMacroFor(longPressKeyBinding);
        break;
    case ButtonState::UNPRESSED:
        if (pendingPresses.erase(button) > 0) {
            executeMacroFor(pressKeyBinding);
        }
        break;
    default:
        break;
    }
}

void KeyBindingExecutor::observe(const DeviceRotaryEncoderEvent& event) {
    executeMacroFor(findKeyBinding(event));
}

std::shared_ptr<KeyBinding> KeyBindingExecutor::findKeyBinding(
    const uint64_t deviceId,
    const uint8_t switchNumber,
    const ButtonState state
) {
    for (const auto& keyBinding : keyBindings) {
        if (keyBinding->trigger->type == PUSH_BUTTON) {
            const auto& trigger = dynamic_cast<PushButtonTrigger&>(*keyBinding->trigger);
            if (trigger.deviceId == deviceId && trigger.number == switchNumber && trigger.state == state) {
                return std::make_shared<KeyBinding>(*keyBinding);
            }
        }
    }

    return nullptr;
}

std::shared_ptr<KeyBinding> KeyBindingExecutor::findKeyBinding(const DeviceRotaryEncoderEvent& event) {
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


void KeyBindingExecutor::executeMacroFor(const std::shared_ptr<KeyBinding>& keyBinding) {
    if (testModeController.isEnabled()) {
        return;
    }

    if (keyBinding == nullptr) {
        return;
    }

    // Read on every execution, so that changes apply right away. Settings are cached, so this is cheap.
    usbConnection.setTypingDelayMs(settings::TypingDelayMs.get(deviceSettings));
    usbConnection.setModifierHoldMs(settings::ModifierHoldMs.get(deviceSettings));

    const auto macro = findCompiledMacro(keyBinding->macroId);
    if (macro == nullptr) {
        return;
    }

    if (!macro->steps.empty()) {
        sequenceRunner.start(macro->macroId, macro->steps);
    } else {
        for (const auto& action : macro->actions) {
            usbConnection.sendAction(*action);
        }
    }
}

void KeyBindingExecutor::executeSequenceStep(const uint16_t macroId) {
    // Macros removed since the sequence was saved are skipped, and so are sequences, which can't be nested
    const auto macro = findCompiledMacro(macroId);
    if (macro == nullptr || !macro->steps.empty()) {
        return;
    }

    for (const auto& action : macro->actions) {
        usbConnection.sendAction(*action);
    }
}

std::shared_ptr<CompiledMacro> KeyBindingExecutor::findCompiledMacro(const uint16_t macroId) {
    for (const auto& macro : macros) {
        if (macro->macroId == macroId) {
            return macro;
        }
    }

    return nullptr;
}

void KeyBindingExecutor::refreshCompiledMacros() {
    macros.clear();

    macroStorage.forEach([&](const Macro& macro) {
        auto compiledMacro = compileMacro(macro);
        if (compiledMacro != nullptr) {
            macros.push_back(compiledMacro);
        }
    });

    logger->info("Reloaded macros. # Found = %i", macros.size());
}

void KeyBindingExecutor::refreshKeyBindings() {
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