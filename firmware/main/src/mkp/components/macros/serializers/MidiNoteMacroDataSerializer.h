#pragma once

#include <memory>
#include <span>
#include <string_view>

#include "mkp/components/macros/MacroDataSerializers.h"
#include "Fields.h"

namespace mkp::components::macros {
    class MidiNoteMacroDataSerializer final
        : public MacroDataStorageSerializer<MidiNoteMacroData> {
    public:
        bool handles(const MacroType type) override {
            return type == MIDI_NOTE;
        };

        std::string_view serialize(const MidiNoteMacroData& data, tfw::utils::allocations::Arena& arena) override {
            return tfw::utils::allocations::arena::strings::sprintf(
                arena,
                "c=%u:n=%u:v=%u:d=%u:r=%d",
                data.channel,
                data.note,
                data.velocity,
                static_cast<unsigned int>(data.durationMs),
                data.endOnRelease ? 1 : 0
            );
        }

        std::shared_ptr<MidiNoteMacroData> deserialize(
            const uint16_t macroId,
            const std::span<const std::string_view>& parts,
            tfw::utils::allocations::Arena& arena
        ) override {
            const auto fields = Fields::parse(parts);
            if (!fields) {
                return nullptr;
            }

            const tfw::utils::allocations::ArenaAllocator<MidiNoteMacroData> allocator(arena);

            // A velocity of zero would be taken as note off by the host
            return std::allocate_shared<MidiNoteMacroData>(
                allocator,
                macroId,
                std::clamp<uint32_t>(fields->unsignedNumber("c", 1), 1, 16),
                std::min<uint32_t>(fields->unsignedNumber("n", 60), 127),
                std::clamp<uint32_t>(fields->unsignedNumber("v", 127), 1, 127),
                std::min(fields->unsignedNumber("d", 0), MaxMidiNoteDurationMs),
                fields->flag("r")
            );
        }
    };
}
