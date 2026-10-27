#pragma once

#include <memory>
#include <span>
#include <string_view>

#include "mkp/components/macros/MacroDataSerializers.h"
#include "Fields.h"

namespace mkp::components::macros {
    class MouseMovementMacroDataSerializer final
        : public MacroDataStorageSerializer<MouseMovementMacroData> {
    public:
        bool handles(const MacroType type) override {
            return type == MOUSE_MOVEMENT;
        };

        std::string_view serialize(const MouseMovementMacroData& data, tfw::utils::allocations::Arena& arena) override {
            return tfw::utils::allocations::arena::strings::sprintf(
                arena,
                "m=0x%02x:x=%d:y=%d",
                data.modifiers,
                data.x,
                data.y
            );
        }

        std::shared_ptr<MouseMovementMacroData> deserialize(
            const uint16_t macroId,
            const std::span<const std::string_view>& parts,
            tfw::utils::allocations::Arena& arena
        ) override {
            const auto fields = Fields::parse(parts);
            if (!fields) {
                return nullptr;
            }

            const tfw::utils::allocations::ArenaAllocator<MouseMovementMacroData> allocator(arena);

            return std::allocate_shared<MouseMovementMacroData>(
                allocator,
                macroId,
                static_cast<uint8_t>(fields->unsignedNumber("m", 0, 16)),
                clampToInt16(fields->number("x", 0)),
                clampToInt16(fields->number("y", 0))
            );
        }
    };
}
