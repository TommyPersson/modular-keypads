#include "MacroDataSerializers.h"

namespace common::macros {
    class ConsumerControlMacroDataSerializer final
        : public MacroDataStorageSerializer<ConsumerControlMacroData> {
    public:
        bool handles(const MacroType type) override {
            return type == CONSUMER_CONTROL;
        };

        std::string_view serialize(const ConsumerControlMacroData& data, utils::allocations::Arena& arena) override {
            return utils::allocations::arena::strings::sprintf(
                arena,
                "0x%04x",
                data.usageId
            );
        }

        std::shared_ptr<ConsumerControlMacroData> deserialize(
            const uint16_t macroId,
            const std::span<const std::string_view>& parts,
            utils::allocations::Arena& arena
        ) override {
            const utils::allocations::ArenaAllocator<ConsumerControlMacroData> allocator(arena);

            const auto usageIdPart = parts[0];

            return std::allocate_shared<ConsumerControlMacroData>(
                allocator,
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

        std::string_view serialize(const ShortcutMacroData& data, utils::allocations::Arena& arena) override {
            return utils::allocations::arena::strings::sprintf(
                arena,
                "0x%02x:0x%02x",
                data.modifiers,
                data.hidKeyCode
            );
        }

        std::shared_ptr<ShortcutMacroData> deserialize(
            const uint16_t macroId,
            const std::span<const std::string_view>& parts,
            utils::allocations::Arena& arena
        ) override {
            const utils::allocations::ArenaAllocator<ShortcutMacroData> allocator(arena);

            const auto modifiersPart = parts[0];
            const auto hidCodePart = parts[1];

            return std::allocate_shared<ShortcutMacroData>(
                allocator,
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

        std::string_view serialize(const SystemControlMacroData& data, utils::allocations::Arena& arena) override {
            return utils::allocations::arena::strings::sprintf(
                arena,
                "0x%02x",
                data.code
            );
        }

        std::shared_ptr<SystemControlMacroData> deserialize(
            const uint16_t macroId,
            const std::span<const std::string_view>& parts,
            utils::allocations::Arena& arena
        ) override {
            const utils::allocations::ArenaAllocator<SystemControlMacroData> allocator(arena);

            const auto codePart = parts[0];

            return std::allocate_shared<SystemControlMacroData>(
                allocator,
                macroId,
                utils::strings::atol(codePart, 16)
            );
        }
    };

    class TextDataSerializer final
        : public MacroDataStorageSerializer<TextMacroData> {
    public:
        bool handles(const MacroType type) override {
            return type == TEXT;
        };

        std::string_view serialize(const TextMacroData& data, utils::allocations::Arena& arena) override {
            return utils::allocations::arena::strings::sprintf(
                arena,
                "%.*s",
                data.text.length(),
                data.text.data()
            );
        }

        std::shared_ptr<TextMacroData> deserialize(
            const uint16_t macroId,
            const std::span<const std::string_view>& parts,
            utils::allocations::Arena& arena
        ) override {
            const utils::allocations::ArenaAllocator<TextMacroData> allocator(arena);

            const auto textPart = parts[0];

            return std::allocate_shared<TextMacroData>(
                allocator,
                macroId,
                textPart
            );
        }
    };

    std::vector<void*> macroDataSerializers{
        new ShortcutMacroDataSerializer{},
        new ConsumerControlMacroDataSerializer{},
        new SystemControlDataSerializer{},
        new TextDataSerializer{},
    };
}
