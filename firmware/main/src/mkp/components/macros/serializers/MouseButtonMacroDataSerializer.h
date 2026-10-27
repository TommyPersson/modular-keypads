#pragma once

#include <memory>
#include <span>
#include <string_view>

#include "mkp/components/macros/MacroDataSerializers.h"
#include "Fields.h"

namespace mkp::components::macros {
    class MouseButtonMacroDataSerializer final
        : public MacroDataStorageSerializer<MouseButtonMacroData> {
    public:
        bool handles(const MacroType type) override {
            return type == MOUSE_BUTTON;
        };

        std::string_view serialize(const MouseButtonMacroData& data, tfw::utils::allocations::Arena& arena) override {
            return tfw::utils::allocations::arena::strings::sprintf(
                arena,
                "m=0x%02x:b=0x%02x",
                data.modifiers,
                data.buttons
            );
        }

        std::shared_ptr<MouseButtonMacroData> deserialize(
            const uint16_t macroId,
            const std::span<const std::string_view>& parts,
            tfw::utils::allocations::Arena& arena
        ) override {
            const auto fields = Fields::parse(parts);
            if (!fields) {
                return nullptr;
            }

            const tfw::utils::allocations::ArenaAllocator<MouseButtonMacroData> allocator(arena);

            return std::allocate_shared<MouseButtonMacroData>(
                allocator,
                macroId,
                static_cast<uint8_t>(fields->unsignedNumber("m", 0, 16)),
                static_cast<uint8_t>(fields->unsignedNumber("b", 0, 16))
            );
        }
    };
}
