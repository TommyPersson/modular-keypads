#pragma once

#include <string>
#include <memory>

namespace mkp::components::macros {
    enum MacroType : uint8_t {
        SHORTCUT = 0x01,
        CONSUMER_CONTROL = 0x02,
        SYSTEM_CONTROL = 0x03,
        TEXT = 0x04,
        MOUSE_SCROLL = 0x05,
        MOUSE_BUTTON = 0x06,
        MOUSE_MOVEMENT = 0x07,
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
        ShortcutMacroData(const uint16_t id, const uint8_t modifiers, const uint8_t hid_key_code)
            : MacroData(id, MacroType::SHORTCUT),
              modifiers(modifiers),
              hidKeyCode(hid_key_code) {
        }

        ~ShortcutMacroData() override = default;

        const uint8_t modifiers;
        const uint8_t hidKeyCode;
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
        MouseScrollMacroData(const uint16_t id, const int8_t wheel, const int8_t pan)
            : MacroData(id, MacroType::MOUSE_SCROLL),
              wheel(wheel),
              pan(pan) {
        }

        ~MouseScrollMacroData() override = default;

        const int8_t wheel; // Positive scrolls up
        const int8_t pan; // Positive scrolls right
    };

    struct MouseButtonMacroData final : MacroData {
        MouseButtonMacroData(const uint16_t id, const uint8_t buttons)
            : MacroData(id, MacroType::MOUSE_BUTTON),
              buttons(buttons) {
        }

        ~MouseButtonMacroData() override = default;

        const uint8_t buttons; // Bit flags: left, right, middle, back, forward
    };

    struct MouseMovementMacroData final : MacroData {
        MouseMovementMacroData(const uint16_t id, const int8_t x, const int8_t y)
            : MacroData(id, MacroType::MOUSE_MOVEMENT),
              x(x),
              y(y) {
        }

        ~MouseMovementMacroData() override = default;

        const int8_t x; // Positive moves right
        const int8_t y; // Positive moves down
    };

    // TODO sequence data

    struct Macro {
        const std::string_view name;
        const std::shared_ptr<MacroData> data;
    };
}
