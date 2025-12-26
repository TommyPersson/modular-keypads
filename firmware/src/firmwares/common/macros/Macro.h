#pragma once

#include <cstdint>

namespace common::macros {
    enum MacroType : uint8_t {
        SHORTCUT = 0x01,
        SEQUENCE = 0x02,
    };

    struct MacroData {
        explicit MacroData(uint16_t id, MacroType type)
            : id(id), type(type) {
        }

        virtual ~MacroData() = default;

        const uint16_t id;
        const MacroType type;
    };

    struct ShortcutMacroData final : MacroData {
        ShortcutMacroData(uint16_t id, uint8_t modifiers, uint8_t hid_key_code)
            : MacroData(id, MacroType::SHORTCUT),
              modifiers(modifiers),
              hidKeyCode(hid_key_code) {
        }

        ~ShortcutMacroData() override = default;

        const uint8_t modifiers;
        const uint8_t hidKeyCode;
    };

    struct Macro {
        const std::string name;
        const std::shared_ptr<MacroData> data;
    };
}
