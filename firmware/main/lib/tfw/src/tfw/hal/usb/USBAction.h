#pragma once

#include <cstdint>
#include <memory>
#include <string_view>
#include <vector>

namespace tfw::hal::usb {
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
        static std::shared_ptr<Action> mouse(
            uint8_t modifiers,
            uint8_t buttons,
            int8_t x,
            int8_t y,
            int8_t wheel,
            int8_t pan
        );
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

    // Relative mouse report. Buttons are clicked (pressed and released) before moving/scrolling. The keyboard
    // modifiers are held during the action, and stay held for consecutive mouse actions with the same modifiers.
    struct MouseAction final : Action {
        MouseAction(
            const uint8_t modifiers,
            const uint8_t buttons,
            const int8_t x,
            const int8_t y,
            const int8_t wheel,
            const int8_t pan
        ) : modifiers(modifiers), buttons(buttons), x(x), y(y), wheel(wheel), pan(pan) {
        }

        ~MouseAction() override = default;

        const uint8_t modifiers; // Bit flags in HID keyboard report order (bit 0 = left control, ...)
        const uint8_t buttons;
        const int8_t x;
        const int8_t y;
        const int8_t wheel;
        const int8_t pan;
    };
}
