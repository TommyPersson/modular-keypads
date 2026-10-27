#pragma once

#include <memory>
#include <span>
#include <string_view>

#include "mkp/components/macros/MacroDataSerializers.h"
#include "Fields.h"

namespace mkp::components::macros {
    class MidiRelativeControlChangeMacroDataSerializer final
        : public MacroDataStorageSerializer<MidiRelativeControlChangeMacroData> {
    public:
        bool handles(const MacroType type) override {
            return type == MIDI_RELATIVE_CONTROL_CHANGE;
        };

        std::string_view serialize(
            const MidiRelativeControlChangeMacroData& data,
            tfw::utils::allocations::Arena& arena
        ) override {
            return tfw::utils::allocations::arena::strings::sprintf(
                arena,
                "c=%u:n=%u:a=%d:e=%u",
                data.channel,
                data.controller,
                data.amount,
                static_cast<unsigned int>(data.encoding)
            );
        }

        std::shared_ptr<MidiRelativeControlChangeMacroData> deserialize(
            const uint16_t macroId,
            const std::span<const std::string_view>& parts,
            tfw::utils::allocations::Arena& arena
        ) override {
            const auto fields = Fields::parse(parts);
            if (!fields) {
                return nullptr;
            }

            // Unknown encodings fall back to the first one
            const auto encodingValue = fields->unsignedNumber("e", 0);
            const auto encoding = encodingValue <= static_cast<uint32_t>(RelativeControlChangeEncoding::SIGN_MAGNITUDE)
                ? static_cast<RelativeControlChangeEncoding>(encodingValue)
                : RelativeControlChangeEncoding::BINARY_OFFSET;

            const tfw::utils::allocations::ArenaAllocator<MidiRelativeControlChangeMacroData> allocator(arena);

            return std::allocate_shared<MidiRelativeControlChangeMacroData>(
                allocator,
                macroId,
                std::clamp<uint32_t>(fields->unsignedNumber("c", 1), 1, 16),
                std::min<uint32_t>(fields->unsignedNumber("n", 0), MaxControlChangeController),
                std::clamp<int>(
                    fields->number("a", 0),
                    -MaxRelativeControlChangeAmount,
                    MaxRelativeControlChangeAmount
                ),
                encoding
            );
        }
    };
}
