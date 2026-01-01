#include "MacroDataSerializers.h"

namespace common::macros {
    class ConsumerControlMacroDataSerializer final
        : public MacroDataStorageSerializer<ConsumerControlMacroData> {
    public:
        bool handles(const MacroType type) override {
            return type == CONSUMER_CONTROL;
        };

        std::string_view serialize(const ConsumerControlMacroData& data, Arena& arena) override {
            return arena::strings::sprintf(
                arena,
                "0x%04x",
                data.usageId
            );
        }

        std::shared_ptr<ConsumerControlMacroData> deserialize(
            const uint16_t macroId,
            const std::span<const std::string_view>& parts,
            Arena& arena
        ) override {
            const auto usageIdPart = parts[0];

            return std::make_shared<ConsumerControlMacroData>(
                macroId,
                utils::strings::atou16(usageIdPart, 16)
            );
        }
    };

    class ShortcutMacroDataSerializer final
        : public MacroDataStorageSerializer<ShortcutMacroData> {
    public:
        bool handles(const MacroType type) override {
            return type == SHORTCUT;
        };

        std::string_view serialize(const ShortcutMacroData& data, Arena& arena) override {
            return arena::strings::sprintf(
                arena,
                "0x%02x:0x%02x",
                data.modifiers,
                data.hidKeyCode
            );
        }

        std::shared_ptr<ShortcutMacroData> deserialize(
            const uint16_t macroId,
            const std::span<const std::string_view>& parts,
            Arena& arena
        ) override {
            const auto modifiersPart = parts[0];
            const auto hidCodePart = parts[1];

            return std::make_shared<ShortcutMacroData>(
                macroId,
                utils::strings::atol(modifiersPart, 16),
                utils::strings::atol(hidCodePart, 16)
            );
        }
    };

    class SystemControlDataSerializer final
        : public MacroDataStorageSerializer<SystemControlMacroData> {
    public:
        bool handles(const MacroType type) override {
            return type == SYSTEM_CONTROL;
        };

        std::string_view serialize(const SystemControlMacroData& data, Arena& arena) override {
            return arena::strings::sprintf(
                arena,
                "0x%02x",
                data.code
            );
        }

        std::shared_ptr<SystemControlMacroData> deserialize(
            const uint16_t macroId,
            const std::span<const std::string_view>& parts,
            Arena& arena
        ) override {
            const auto codePart = parts[0];

            return std::make_shared<SystemControlMacroData>(
                macroId,
                utils::strings::atol(codePart, 16)
            );
        }
    };

    std::vector<void*> macroDataSerializers{
        new ShortcutMacroDataSerializer{},
        new ConsumerControlMacroDataSerializer{},
        new SystemControlDataSerializer{},
    };
}
