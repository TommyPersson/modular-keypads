#pragma once

#include <memory>
#include <span>
#include <string_view>

#include "mkp/components/macros/MacroDataSerializers.h"
#include "Fields.h"

namespace mkp::components::macros {
    class MouseScrollMacroDataSerializer final
        : public MacroDataStorageSerializer<MouseScrollMacroData> {
    public:
        bool handles(const MacroType type) override {
            return type == MOUSE_SCROLL;
        };

        std::string_view serialize(const MouseScrollMacroData& data, tfw::utils::allocations::Arena& arena) override {
            return tfw::utils::allocations::arena::strings::sprintf(
                arena,
                "m=0x%02x:w=%d:p=%d",
                data.modifiers,
                data.wheel,
                data.pan
            );
        }

        std::shared_ptr<MouseScrollMacroData> deserialize(
            const uint16_t macroId,
            const std::span<const std::string_view>& parts,
            tfw::utils::allocations::Arena& arena
        ) override {
            const auto fields = Fields::parse(parts);
            if (!fields) {
                return nullptr;
            }

            const tfw::utils::allocations::ArenaAllocator<MouseScrollMacroData> allocator(arena);

            return std::allocate_shared<MouseScrollMacroData>(
                allocator,
                macroId,
                static_cast<uint8_t>(fields->unsignedNumber("m", 0, 16)),
                clampToInt8(fields->number("w", 0)),
                clampToInt8(fields->number("p", 0))
            );
        }
    };
}
