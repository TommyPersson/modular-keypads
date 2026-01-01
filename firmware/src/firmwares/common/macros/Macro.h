#pragma once

#include <string>
#include <memory>

namespace common::macros {
    enum MacroType : uint8_t {
        SHORTCUT = 0x01,
        CONSUMER_CONTROL = 0x02,
        SYSTEM_CONTROL = 0x03,
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

    // TODO sequence data

    struct Macro {
        const std::string name;
        const std::shared_ptr<MacroData> data;
    };
}
