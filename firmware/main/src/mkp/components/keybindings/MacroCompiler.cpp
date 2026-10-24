#include "MacroCompiler.h"

#include <tfw/utils/strings.h>

using namespace mkp::components::macros;
using namespace mkp::components::keybindings;

std::shared_ptr<CompiledMacro> mkp::components::keybindings::compileMacro(const Macro& macro) {
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

    if (macro.data->type == MIDI_NOTE) {
        const auto& data = dynamic_cast<MidiNoteMacroData&>(*macro.data);
        if (data.endOnRelease) {
            return std::make_shared<CompiledMacro>(CompiledMacro{
                .macroId = macro.data->id,
                .actions = std::vector{tfw::hal::usb::Action::heldMidiNote(data.channel, data.note, data.velocity)},
                .steps = {},
                .releaseActions = std::vector{tfw::hal::usb::Action::midiNoteOff(data.channel, data.note)},
            });
        }

        const auto action = tfw::hal::usb::Action::midiNote(data.channel, data.note, data.velocity, data.durationMs);
        return std::make_shared<CompiledMacro>(CompiledMacro{
            .macroId = macro.data->id,
            .actions = std::vector{action},
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
