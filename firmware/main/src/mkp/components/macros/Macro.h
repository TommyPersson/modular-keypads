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
        MIDI_RELATIVE_CONTROL_CHANGE = 0x0a,
        MIDI_ABSOLUTE_CONTROL_CHANGE = 0x0b,
        RANDOM_TEXT = 0x0c,
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

    // How relative changes are encoded in control change values, as apps expect different ones
    enum class RelativeControlChangeEncoding : uint8_t {
        BINARY_OFFSET = 0, // 64 + amount: 65 is +1, 63 is -1
        TWOS_COMPLEMENT = 1, // 1 is +1, 127 is -1
        SIGN_MAGNITUDE = 2, // 1 is +1, 65 is -1
    };

    // Largest amount that all the encodings can represent
    constexpr int8_t MaxRelativeControlChangeAmount = 63;
    constexpr uint8_t MaxControlChangeController = 119;

    // Changes the value of a controller by an amount, such as for an app slider turned with a rotary encoder
    struct MidiRelativeControlChangeMacroData final : MacroData {
        MidiRelativeControlChangeMacroData(
            const uint16_t id,
            const uint8_t channel,
            const uint8_t controller,
            const int8_t amount,
            const RelativeControlChangeEncoding encoding
        ) : MacroData(id, MacroType::MIDI_RELATIVE_CONTROL_CHANGE),
            channel(channel),
            controller(controller),
            amount(amount),
            encoding(encoding) {
        }

        ~MidiRelativeControlChangeMacroData() override = default;

        const uint8_t channel; // 1 to 16
        const uint8_t controller; // 0 to 119
        const int8_t amount; // -63 to 63
        const RelativeControlChangeEncoding encoding;
    };

    // Steps the value of a controller by an amount, and sends the new value, for apps that only take absolute values.
    // The device keeps the value, starting at zero, as it doesn't know the value in the app.
    struct MidiAbsoluteControlChangeMacroData final : MacroData {
        MidiAbsoluteControlChangeMacroData(
            const uint16_t id,
            const uint8_t channel,
            const uint8_t controller,
            const int8_t amount
        ) : MacroData(id, MacroType::MIDI_ABSOLUTE_CONTROL_CHANGE),
            channel(channel),
            controller(controller),
            amount(amount) {
        }

        ~MidiAbsoluteControlChangeMacroData() override = default;

        const uint8_t channel; // 1 to 16
        const uint8_t controller; // 0 to 119
        const int8_t amount; // -127 to 127
    };

    // What kind of random text is typed. Stored by number, so the numbers must stay the same.
    enum class RandomTextFormat : uint8_t {
        UUID_V4 = 1, // Lowercase with hyphens: 0f8fad5b-d9cb-469f-a165-70867728950e
    };

    // Types random text that's generated each time the macro runs, such as a new ID
    struct RandomTextMacroData final : MacroData {
        RandomTextMacroData(const uint16_t id, const RandomTextFormat format, const bool quoted = false)
            : MacroData(id, MacroType::RANDOM_TEXT),
              format(format),
              quoted(quoted) {
        }

        ~RandomTextMacroData() override = default;

        const RandomTextFormat format;
        // Wraps the text in double quotes, such as for pasting into code or JSON
        const bool quoted;
    };

    // Runs a macro, or for loop steps, goes back to an earlier step a number of times, such as to repeat a few steps
    struct SequenceStep {
        // Zero for loop steps
        uint16_t macroId;
        // Time to wait before running the macro, counted from when the previous step is done typing. For loop steps,
        // the time to wait before going back, which isn't waited once it's done going back.
        uint32_t delayMs;
        // For loop steps, the index of the earlier step to go back to
        uint8_t loopTarget = 0;
        // For loop steps, how many times to go back before going on with the next step. Zero for other steps.
        uint16_t loopRepeats = 0;

        bool isLoop() const {
            return loopRepeats > 0;
        }
    };

    // Limits that keep a stored sequence within a command line and a stored line, and its steps reasonable
    constexpr size_t MaxSequenceSteps = 32;
    constexpr uint32_t MaxSequenceStepDelayMs = 60'000;
    constexpr uint16_t MaxSequenceLoopRepeats = 1'000;

    // Runs other macros in order. Sequences can't be steps of other sequences, and loops only go back to earlier steps
    // a limited number of times, which keeps them from running forever.
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
