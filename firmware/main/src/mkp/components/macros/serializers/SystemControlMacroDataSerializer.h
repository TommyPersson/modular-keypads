#pragma once

#include <memory>
#include <span>
#include <string_view>

#include "mkp/components/macros/MacroDataSerializers.h"
#include "Fields.h"

namespace mkp::components::macros {
    class SystemControlMacroDataSerializer final
        : public MacroDataStorageSerializer<SystemControlMacroData> {
    public:
        bool handles(const MacroType type) override {
            return type == SYSTEM_CONTROL;
        };

        std::string_view serialize(const SystemControlMacroData& data, tfw::utils::allocations::Arena& arena) override {
            return tfw::utils::allocations::arena::strings::sprintf(
                arena,
                "c=0x%02x",
                data.code
            );
        }

        std::shared_ptr<SystemControlMacroData> deserialize(
            const uint16_t macroId,
            const std::span<const std::string_view>& parts,
            tfw::utils::allocations::Arena& arena
        ) override {
            const auto fields = Fields::parse(parts);
            if (!fields) {
                return nullptr;
            }

            const tfw::utils::allocations::ArenaAllocator<SystemControlMacroData> allocator(arena);

            return std::allocate_shared<SystemControlMacroData>(
                allocator,
                macroId,
                fields->number("c", 0, 16)
            );
        }
    };
}
