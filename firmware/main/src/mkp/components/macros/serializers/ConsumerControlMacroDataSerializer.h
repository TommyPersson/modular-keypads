#pragma once

#include <memory>
#include <span>
#include <string_view>

#include "mkp/components/macros/MacroDataSerializers.h"
#include "Fields.h"

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
                "u=0x%04x",
                data.usageId
            );
        }

        std::shared_ptr<ConsumerControlMacroData> deserialize(
            const uint16_t macroId,
            const std::span<const std::string_view>& parts,
            tfw::utils::allocations::Arena& arena
        ) override {
            const auto fields = Fields::parse(parts);
            if (!fields) {
                return nullptr;
            }

            const tfw::utils::allocations::ArenaAllocator<ConsumerControlMacroData> allocator(arena);

            return std::allocate_shared<ConsumerControlMacroData>(
                allocator,
                macroId,
                static_cast<uint16_t>(fields->unsignedNumber("u", 0, 16))
            );
        }
    };
}
