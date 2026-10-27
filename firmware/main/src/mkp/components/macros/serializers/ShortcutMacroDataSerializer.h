#pragma once

#include <memory>
#include <span>
#include <string_view>

#include "mkp/components/macros/MacroDataSerializers.h"
#include "mkp/components/settings/Settings.h"
#include "Fields.h"

namespace mkp::components::macros {
    class ShortcutMacroDataSerializer final
        : public MacroDataStorageSerializer<ShortcutMacroData> {
    public:
        bool handles(const MacroType type) override {
            return type == SHORTCUT;
        };

        std::string_view serialize(const ShortcutMacroData& data, tfw::utils::allocations::Arena& arena) override {
            return tfw::utils::allocations::arena::strings::sprintf(
                arena,
                "m=0x%02x:k=0x%02x:h=%d:t=%u",
                data.modifiers,
                data.hidKeyCode,
                data.holdModifiers ? 1 : 0,
                static_cast<unsigned int>(data.holdTimeMs)
            );
        }

        std::shared_ptr<ShortcutMacroData> deserialize(
            const uint16_t macroId,
            const std::span<const std::string_view>& parts,
            tfw::utils::allocations::Arena& arena
        ) override {
            const auto fields = Fields::parse(parts);
            if (!fields) {
                return nullptr;
            }

            const tfw::utils::allocations::ArenaAllocator<ShortcutMacroData> allocator(arena);

            return std::allocate_shared<ShortcutMacroData>(
                allocator,
                macroId,
                fields->number("m", 0, 16),
                fields->number("k", 0, 16),
                fields->flag("h"),
                std::min(fields->unsignedNumber("t", 0), settings::ModifierHoldMs.maxValue)
            );
        }
    };
}
