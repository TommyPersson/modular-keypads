#include "MacroDataSerializers.h"

#include <algorithm>

namespace mkp::components::macros {
    class ConsumerControlMacroDataSerializer final
        : public MacroDataStorageSerializer<ConsumerControlMacroData> {
    public:
        bool handles(const MacroType type) override {
            return type == CONSUMER_CONTROL;
        };

        std::string_view serialize(const ConsumerControlMacroData& data, tfw::utils::allocations::Arena& arena) override {
            return tfw::utils::allocations::arena::strings::sprintf(
                arena,
                "0x%04x",
                data.usageId
            );
        }

        std::shared_ptr<ConsumerControlMacroData> deserialize(
            const uint16_t macroId,
            const std::span<const std::string_view>& parts,
            tfw::utils::allocations::Arena& arena
        ) override {
            const tfw::utils::allocations::ArenaAllocator<ConsumerControlMacroData> allocator(arena);

            const auto usageIdPart = parts[0];

            return std::allocate_shared<ConsumerControlMacroData>(
                allocator,
                macroId,
                tfw::utils::strings::atou16(usageIdPart, 16)
            );
        }
    };

    class ShortcutMacroDataSerializer final
        : public MacroDataStorageSerializer<ShortcutMacroData> {
    public:
        bool handles(const MacroType type) override {
            return type == SHORTCUT;
        };

        std::string_view serialize(const ShortcutMacroData& data, tfw::utils::allocations::Arena& arena) override {
            return tfw::utils::allocations::arena::strings::sprintf(
                arena,
                "0x%02x:0x%02x",
                data.modifiers,
                data.hidKeyCode
            );
        }

        std::shared_ptr<ShortcutMacroData> deserialize(
            const uint16_t macroId,
            const std::span<const std::string_view>& parts,
            tfw::utils::allocations::Arena& arena
        ) override {
            const tfw::utils::allocations::ArenaAllocator<ShortcutMacroData> allocator(arena);

            const auto modifiersPart = parts[0];
            const auto hidCodePart = parts[1];

            return std::allocate_shared<ShortcutMacroData>(
                allocator,
                macroId,
                tfw::utils::strings::atol(modifiersPart, 16),
                tfw::utils::strings::atol(hidCodePart, 16)
            );
        }
    };

    class SystemControlDataSerializer final
        : public MacroDataStorageSerializer<SystemControlMacroData> {
    public:
        bool handles(const MacroType type) override {
            return type == SYSTEM_CONTROL;
        };

        std::string_view serialize(const SystemControlMacroData& data, tfw::utils::allocations::Arena& arena) override {
            return tfw::utils::allocations::arena::strings::sprintf(
                arena,
                "0x%02x",
                data.code
            );
        }

        std::shared_ptr<SystemControlMacroData> deserialize(
            const uint16_t macroId,
            const std::span<const std::string_view>& parts,
            tfw::utils::allocations::Arena& arena
        ) override {
            const tfw::utils::allocations::ArenaAllocator<SystemControlMacroData> allocator(arena);

            const auto codePart = parts[0];

            return std::allocate_shared<SystemControlMacroData>(
                allocator,
                macroId,
                tfw::utils::strings::atol(codePart, 16)
            );
        }
    };

    class TextDataSerializer final
        : public MacroDataStorageSerializer<TextMacroData> {
    public:
        bool handles(const MacroType type) override {
            return type == TEXT;
        };

        std::string_view serialize(const TextMacroData& data, tfw::utils::allocations::Arena& arena) override {
            return tfw::utils::allocations::arena::strings::sprintf(
                arena,
                "%.*s",
                static_cast<int>(data.text.length()),
                data.text.data()
            );
        }

        std::shared_ptr<TextMacroData> deserialize(
            const uint16_t macroId,
            const std::span<const std::string_view>& parts,
            tfw::utils::allocations::Arena& arena
        ) override {
            const tfw::utils::allocations::ArenaAllocator<TextMacroData> allocator(arena);

            const auto textPart = parts[0];

            return std::allocate_shared<TextMacroData>(
                allocator,
                macroId,
                textPart
            );
        }
    };

    static int8_t clampToInt8(const int value) {
        return static_cast<int8_t>(std::clamp(value, -127, 127));
    }

    // Keyboard modifiers of mouse macros come last, as they were added later. Macros saved before have none.
    static uint8_t parseOptionalModifiers(const std::span<const std::string_view>& parts, const size_t index) {
        return parts.size() > index ? tfw::utils::strings::atou8(parts[index], 16) : 0;
    }

    class MouseScrollDataSerializer final
        : public MacroDataStorageSerializer<MouseScrollMacroData> {
    public:
        bool handles(const MacroType type) override {
            return type == MOUSE_SCROLL;
        };

        std::string_view serialize(const MouseScrollMacroData& data, tfw::utils::allocations::Arena& arena) override {
            return tfw::utils::allocations::arena::strings::sprintf(
                arena,
                "%d:%d:0x%02x",
                data.wheel,
                data.pan,
                data.modifiers
            );
        }

        std::shared_ptr<MouseScrollMacroData> deserialize(
            const uint16_t macroId,
            const std::span<const std::string_view>& parts,
            tfw::utils::allocations::Arena& arena
        ) override {
            if (parts.size() < 2) {
                return nullptr;
            }

            const tfw::utils::allocations::ArenaAllocator<MouseScrollMacroData> allocator(arena);

            return std::allocate_shared<MouseScrollMacroData>(
                allocator,
                macroId,
                parseOptionalModifiers(parts, 2),
                clampToInt8(tfw::utils::strings::atol(parts[0])),
                clampToInt8(tfw::utils::strings::atol(parts[1]))
            );
        }
    };

    class MouseButtonDataSerializer final
        : public MacroDataStorageSerializer<MouseButtonMacroData> {
    public:
        bool handles(const MacroType type) override {
            return type == MOUSE_BUTTON;
        };

        std::string_view serialize(const MouseButtonMacroData& data, tfw::utils::allocations::Arena& arena) override {
            return tfw::utils::allocations::arena::strings::sprintf(
                arena,
                "0x%02x:0x%02x",
                data.buttons,
                data.modifiers
            );
        }

        std::shared_ptr<MouseButtonMacroData> deserialize(
            const uint16_t macroId,
            const std::span<const std::string_view>& parts,
            tfw::utils::allocations::Arena& arena
        ) override {
            if (parts.empty()) {
                return nullptr;
            }

            const tfw::utils::allocations::ArenaAllocator<MouseButtonMacroData> allocator(arena);

            return std::allocate_shared<MouseButtonMacroData>(
                allocator,
                macroId,
                parseOptionalModifiers(parts, 1),
                tfw::utils::strings::atou8(parts[0], 16)
            );
        }
    };

    class MouseMovementDataSerializer final
        : public MacroDataStorageSerializer<MouseMovementMacroData> {
    public:
        bool handles(const MacroType type) override {
            return type == MOUSE_MOVEMENT;
        };

        std::string_view serialize(const MouseMovementMacroData& data, tfw::utils::allocations::Arena& arena) override {
            return tfw::utils::allocations::arena::strings::sprintf(
                arena,
                "%d:%d:0x%02x",
                data.x,
                data.y,
                data.modifiers
            );
        }

        std::shared_ptr<MouseMovementMacroData> deserialize(
            const uint16_t macroId,
            const std::span<const std::string_view>& parts,
            tfw::utils::allocations::Arena& arena
        ) override {
            if (parts.size() < 2) {
                return nullptr;
            }

            const tfw::utils::allocations::ArenaAllocator<MouseMovementMacroData> allocator(arena);

            return std::allocate_shared<MouseMovementMacroData>(
                allocator,
                macroId,
                parseOptionalModifiers(parts, 2),
                clampToInt8(tfw::utils::strings::atol(parts[0])),
                clampToInt8(tfw::utils::strings::atol(parts[1]))
            );
        }
    };

    std::vector<void*> macroDataSerializers{
        new ShortcutMacroDataSerializer{},
        new ConsumerControlMacroDataSerializer{},
        new SystemControlDataSerializer{},
        new TextDataSerializer{},
        new MouseScrollDataSerializer{},
        new MouseButtonDataSerializer{},
        new MouseMovementDataSerializer{},
    };
}
