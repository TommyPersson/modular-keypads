#pragma once

#include <memory>
#include <span>
#include <string_view>

#include "mkp/components/macros/MacroDataSerializers.h"
#include "Fields.h"

namespace mkp::components::macros {
    class TextMacroDataSerializer final
        : public MacroDataStorageSerializer<TextMacroData> {
    public:
        bool handles(const MacroType type) override {
            return type == TEXT;
        };

        std::string_view serialize(const TextMacroData& data, tfw::utils::allocations::Arena& arena) override {
            return tfw::utils::allocations::arena::strings::sprintf(
                arena,
                "t=%.*s",
                static_cast<int>(data.text.length()),
                data.text.data()
            );
        }

        std::shared_ptr<TextMacroData> deserialize(
            const uint16_t macroId,
            const std::span<const std::string_view>& parts,
            tfw::utils::allocations::Arena& arena
        ) override {
            // The text is URI encoded, so it has no separators of its own
            const auto fields = Fields::parse(parts);
            if (!fields) {
                return nullptr;
            }

            const tfw::utils::allocations::ArenaAllocator<TextMacroData> allocator(arena);

            return std::allocate_shared<TextMacroData>(
                allocator,
                macroId,
                fields->get("t").value_or("")
            );
        }
    };
}
