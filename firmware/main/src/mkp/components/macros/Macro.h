#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace mkp::components::macros {
    enum MacroType : uint8_t {
        SHORTCUT = 0x01,
        CONSUMER_CONTROL = 0x02,
        SYSTEM_CONTROL = 0x03,
        TEXT = 0x04,
        MOUSE_SCROLL = 0x05,
        MOUSE_BUTTON = 0x06,
        MOUSE_MOVEMENT = 0x07,
        SEQUENCE = 0x08,
        MIDI_NOTE = 0x09,
    };

    struct MacroData {
        explicit MacroData(const uint16_t id, const MacroType type)
            : id(id), type(type) {
        }

        virtual ~MacroData() = default;

        uint16_t id;
        const MacroType type;
    };

    struct ShortcutMacroData final : MacroData {
        ShortcutMacroData(
            const uint16_t id,
            const uint8_t modifiers,
            const uint8_t hid_key_code,
            const bool holdModifiers = false,
            const uint32_t holdTimeMs = 0
        ) : MacroData(id, MacroType::SHORTCUT),
            modifiers(modifiers),
            hidKeyCode(hid_key_code),
            holdModifiers(holdModifiers),
            holdTimeMs(holdTimeMs) {
        }

        ~ShortcutMacroData() override = default;

        const uint8_t modifiers;
        const uint8_t hidKeyCode;
        // Keeps the modifiers held between repeats of the shortcut, such as for Alt + Tab
        const bool holdModifiers;
        // How long the modifiers stay held after the last repeat. Zero uses the modifier hold time setting.
        const uint32_t holdTimeMs;
    };

    struct ConsumerControlMacroData final : MacroData {
        ConsumerControlMacroData(const uint16_t id, const uint16_t usageId)
            : MacroData(id, MacroType::CONSUMER_CONTROL),
              usageId(usageId) {
        }

        ~ConsumerControlMacroData() override = default;

        const uint16_t usageId;
    };

    struct SystemControlMacroData final : MacroData {
        SystemControlMacroData(const uint16_t id, const uint8_t code)
            : MacroData(id, MacroType::SYSTEM_CONTROL),
              code(code) {
        }

        ~SystemControlMacroData() override = default;

        const uint8_t code;
    };

    struct TextMacroData final : MacroData {
        TextMacroData(const uint16_t id, const std::string_view& text)
            : MacroData(id, MacroType::TEXT),
              text(text) {
        }

        ~TextMacroData() override = default;

        const std::string_view text;
    };

    struct MouseScrollMacroData final : MacroData {
        MouseScrollMacroData(const uint16_t id, const uint8_t modifiers, const int8_t wheel, const int8_t pan)
            : MacroData(id, MacroType::MOUSE_SCROLL),
              modifiers(modifiers),
              wheel(wheel),
              pan(pan) {
        }

        ~MouseScrollMacroData() override = default;

        const uint8_t modifiers; // Same flags as for shortcuts
        const int8_t wheel; // Positive scrolls up
        const int8_t pan; // Positive scrolls right
    };

    struct MouseButtonMacroData final : MacroData {
        MouseButtonMacroData(const uint16_t id, const uint8_t modifiers, const uint8_t buttons)
            : MacroData(id, MacroType::MOUSE_BUTTON),
              modifiers(modifiers),
              buttons(buttons) {
        }

        ~MouseButtonMacroData() override = default;

        const uint8_t modifiers; // Same flags as for shortcuts
        const uint8_t buttons; // Bit flags: left, right, middle, back, forward
    };

    struct MouseMovementMacroData final : MacroData {
        MouseMovementMacroData(const uint16_t id, const uint8_t modifiers, const int16_t x, const int16_t y)
            : MacroData(id, MacroType::MOUSE_MOVEMENT),
              modifiers(modifiers),
              x(x),
              y(y) {
        }

        ~MouseMovementMacroData() override = default;

        const uint8_t modifiers; // Same flags as for shortcuts
        const int16_t x; // Positive moves right
        const int16_t y; // Positive moves down
    };

    constexpr uint32_t MaxMidiNoteDurationMs = 60'000;

    struct MidiNoteMacroData final : MacroData {
        MidiNoteMacroData(
            const uint16_t id,
            const uint8_t channel,
            const uint8_t note,
            const uint8_t velocity,
            const uint32_t durationMs,
            const bool endOnRelease = false
        ) : MacroData(id, MacroType::MIDI_NOTE),
            channel(channel),
            note(note),
            velocity(velocity),
            durationMs(durationMs),
            endOnRelease(endOnRelease) {
        }

        ~MidiNoteMacroData() override = default;

        const uint8_t channel; // 1 to 16
        const uint8_t note; // 0 to 127, 60 is middle C
        const uint8_t velocity; // 1 to 127
        const uint32_t durationMs; // Time between note on and note off, unless it ends on release
        // Plays the note until the button is released, rather than for the duration
        const bool endOnRelease;
    };

    struct SequenceStep {
        uint16_t macroId;
        // Time to wait before running the macro, counted from when the previous step is done typing
        uint32_t delayMs;
    };

    // Limits that keep a stored sequence within a command line and a stored line, and its steps reasonable
    constexpr size_t MaxSequenceSteps = 32;
    constexpr uint32_t MaxSequenceStepDelayMs = 60'000;

    // Runs other macros in order. Sequences can't be steps of other sequences, which keeps them from running forever.
    struct SequenceMacroData final : MacroData {
        SequenceMacroData(const uint16_t id, std::vector<SequenceStep> steps)
            : MacroData(id, MacroType::SEQUENCE),
              steps(std::move(steps)) {
        }

        ~SequenceMacroData() override = default;

        const std::vector<SequenceStep> steps;
    };

    struct Macro {
        const std::string_view name;
        const std::shared_ptr<MacroData> data;
    };
}
