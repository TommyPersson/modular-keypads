#pragma once

#include <memory>
#include <span>
#include <string_view>

#include "mkp/components/macros/MacroDataSerializers.h"
#include "mkp/components/generators/RandomTextGenerator.h"
#include "Fields.h"

namespace mkp::components::macros {
    class RandomTextMacroDataSerializer final
        : public MacroDataStorageSerializer<RandomTextMacroData> {
    public:
        bool handles(const MacroType type) override {
            return type == RANDOM_TEXT;
        };

        std::string_view serialize(const RandomTextMacroData& data, tfw::utils::allocations::Arena& arena) override {
            return tfw::utils::allocations::arena::strings::sprintf(
                arena,
                "f=%u:q=%d",
                static_cast<unsigned int>(data.format),
                data.quoted ? 1 : 0
            );
        }

        std::shared_ptr<RandomTextMacroData> deserialize(
            const uint16_t macroId,
            const std::span<const std::string_view>& parts,
            tfw::utils::allocations::Arena& arena
        ) override {
            const auto fields = Fields::parse(parts);
            if (!fields) {
                return nullptr;
            }

            // Unknown formats, such as ones added in later firmware, are ignored
            const auto format = static_cast<RandomTextFormat>(fields->unsignedNumber("f", 0));
            if (generators::RandomTextGenerator::forFormat(format) == nullptr) {
                return nullptr;
            }

            const tfw::utils::allocations::ArenaAllocator<RandomTextMacroData> allocator(arena);

            return std::allocate_shared<RandomTextMacroData>(
                allocator,
                macroId,
                format,
                fields->flag("q")
            );
        }
    };
}
