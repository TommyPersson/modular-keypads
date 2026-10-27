#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "tfw/utils/allocations/Arena.h"

namespace tfw::hal::usb {
    // Writes generated text into the arena, and results in a view of it. Empty when the arena is too small.
    using TextGenerator = std::function<std::string_view(utils::allocations::Arena& arena)>;

    // TODO support custom allocator

    struct Action {
        virtual ~Action() = default;

        static std::shared_ptr<Action> shortcut(
            uint8_t modifiers,
            uint8_t keyCode,
            bool holdModifiers,
            uint32_t holdTimeMs
        );
        static std::shared_ptr<Action> consumerControl(uint16_t usageId);
        static std::shared_ptr<Action> systemControl(uint8_t code);
        static std::shared_ptr<Action> type(const std::string_view& text);
        static std::shared_ptr<Action> typeGenerated(TextGenerator generate);
        static std::shared_ptr<Action> mouse(
            uint8_t modifiers,
            uint8_t buttons,
            int16_t x,
            int16_t y,
            int8_t wheel,
            int8_t pan
        );
        static std::shared_ptr<Action> midiNote(uint8_t channel, uint8_t note, uint8_t velocity, uint32_t durationMs);
        static std::shared_ptr<Action> heldMidiNote(uint8_t channel, uint8_t note, uint8_t velocity);
        static std::shared_ptr<Action> midiNoteOff(uint8_t channel, uint8_t note);
        static std::shared_ptr<Action> midiControlChange(uint8_t channel, uint8_t controller, uint8_t value);
        static std::shared_ptr<Action> midiControlChangeStep(uint8_t channel, uint8_t controller, int8_t amount);
    };

    struct ConsumerControlAction final : Action {
        explicit ConsumerControlAction(const uint16_t usageId) : usageId(usageId) {
        }

        ~ConsumerControlAction() override = default;

        const uint16_t usageId;
    };

    struct SystemControlAction final : Action {
        explicit SystemControlAction(const uint8_t code) : code(code) {
        }

        ~SystemControlAction() override = default;

        const uint8_t code;
    };

    // A key press combined with keyboard modifiers. With holdModifiers, the modifiers stay held after the key press,
    // for consecutive actions with the same modifiers. Repeating Alt + Tab then keeps the window switcher open, and
    // switches when the repeats stop.
    struct ShortcutAction final : Action {
        ShortcutAction(const uint8_t modifiers, const uint8_t keyCode, const bool holdModifiers, const uint32_t holdTimeMs)
            : modifiers(modifiers), keyCode(keyCode), holdModifiers(holdModifiers), holdTimeMs(holdTimeMs) {
        }

        ~ShortcutAction() override = default;

        const uint8_t modifiers; // Bit flags in HID keyboard report order (bit 0 = left control, ...)
        const uint8_t keyCode;
        const bool holdModifiers;
        const uint32_t holdTimeMs; // Zero uses the connection's modifier hold time
    };

    struct TypeAction final : Action {
        ~TypeAction() override = default;

        explicit TypeAction(const std::string_view& text) : text(text) {
        }

        const std::string text;
    };

    // Types text that's generated each time the action is sent, such as a random ID that differs every time. The text
    // is generated into an arena of the sender, rather than allocated, so that repeated sends don't fragment the heap.
    struct GeneratedTypeAction final : Action {
        ~GeneratedTypeAction() override = default;

        explicit GeneratedTypeAction(TextGenerator generate) : generate(std::move(generate)) {
        }

        const TextGenerator generate;
    };

    // Relative mouse input. Buttons are clicked (pressed and released) before moving/scrolling. Movement beyond the
    // range of a single report is sent as several reports. The keyboard modifiers are held during the action, and stay
    // held for consecutive mouse actions with the same modifiers.
    struct MouseAction final : Action {
        MouseAction(
            const uint8_t modifiers,
            const uint8_t buttons,
            const int16_t x,
            const int16_t y,
            const int8_t wheel,
            const int8_t pan
        ) : modifiers(modifiers), buttons(buttons), x(x), y(y), wheel(wheel), pan(pan) {
        }

        ~MouseAction() override = default;

        const uint8_t modifiers; // Bit flags in HID keyboard report order (bit 0 = left control, ...)
        const uint8_t buttons;
        const int16_t x;
        const int16_t y;
        const int8_t wheel;
        const int8_t pan;
    };

    // A MIDI note, played for a duration: note on right away, and note off once the duration has passed. Held notes
    // have no duration, and play until a note off action for them. Playing the same note again before it has ended
    // ends the earlier one first.
    struct MidiNoteAction final : Action {
        MidiNoteAction(
            const uint8_t channel,
            const uint8_t note,
            const uint8_t velocity,
            const uint32_t durationMs,
            const bool held
        ) : channel(channel), note(note), velocity(velocity), durationMs(durationMs), held(held) {
        }

        ~MidiNoteAction() override = default;

        const uint8_t channel; // 1 to 16
        const uint8_t note; // 0 to 127, 60 is middle C
        const uint8_t velocity; // 1 to 127
        const uint32_t durationMs; // Zero sends note off right after note on. Not used by held notes.
        const bool held;
    };

    // Ends a playing MIDI note, such as a held one. Notes that have already ended are left alone.
    struct MidiNoteOffAction final : Action {
        MidiNoteOffAction(const uint8_t channel, const uint8_t note) : channel(channel), note(note) {
        }

        ~MidiNoteOffAction() override = default;

        const uint8_t channel;
        const uint8_t note;
    };

    struct MidiControlChangeAction final : Action {
        MidiControlChangeAction(const uint8_t channel, const uint8_t controller, const uint8_t value)
            : channel(channel), controller(controller), value(value) {
        }

        ~MidiControlChangeAction() override = default;

        const uint8_t channel; // 1 to 16
        const uint8_t controller; // 0 to 119, as 120 to 127 are channel mode messages
        const uint8_t value; // 0 to 127
    };

    // Changes the value of a controller by an amount, and sends the new value. The value is kept by the sender, per
    // channel and controller, starting at zero, and is limited to 0 to 127.
    struct MidiControlChangeStepAction final : Action {
        MidiControlChangeStepAction(const uint8_t channel, const uint8_t controller, const int8_t amount)
            : channel(channel), controller(controller), amount(amount) {
        }

        ~MidiControlChangeStepAction() override = default;

        const uint8_t channel; // 1 to 16
        const uint8_t controller; // 0 to 119
        const int8_t amount;
    };
}
