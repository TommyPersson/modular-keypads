#pragma once

#include <memory>
#include <span>
#include <string_view>

#include "mkp/components/macros/MacroDataSerializers.h"
#include "Fields.h"

namespace mkp::components::macros {
    class MidiAbsoluteControlChangeMacroDataSerializer final
        : public MacroDataStorageSerializer<MidiAbsoluteControlChangeMacroData> {
    public:
        bool handles(const MacroType type) override {
            return type == MIDI_ABSOLUTE_CONTROL_CHANGE;
        };

        std::string_view serialize(
            const MidiAbsoluteControlChangeMacroData& data,
            tfw::utils::allocations::Arena& arena
        ) override {
            return tfw::utils::allocations::arena::strings::sprintf(
                arena,
                "c=%u:n=%u:a=%d",
                data.channel,
                data.controller,
                data.amount
            );
        }

        std::shared_ptr<MidiAbsoluteControlChangeMacroData> deserialize(
            const uint16_t macroId,
            const std::span<const std::string_view>& parts,
            tfw::utils::allocations::Arena& arena
        ) override {
            const auto fields = Fields::parse(parts);
            if (!fields) {
                return nullptr;
            }

            const tfw::utils::allocations::ArenaAllocator<MidiAbsoluteControlChangeMacroData> allocator(arena);

            return std::allocate_shared<MidiAbsoluteControlChangeMacroData>(
                allocator,
                macroId,
                std::clamp<uint32_t>(fields->unsignedNumber("c", 1), 1, 16),
                std::min<uint32_t>(fields->unsignedNumber("n", 0), MaxControlChangeController),
                clampToInt8(fields->number("a", 0))
            );
        }
    };
}
