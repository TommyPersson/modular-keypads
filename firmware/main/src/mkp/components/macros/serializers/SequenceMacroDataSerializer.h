#pragma once

#include <algorithm>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "mkp/components/macros/MacroDataSerializers.h"
#include "Fields.h"

namespace mkp::components::macros {
    // The steps are one field, separated by semicolons, with the fields of each step separated by slashes. A step
    // field is a letter followed by a number: the macro to run (m) and the delay (d), or for loops, the index of the
    // step to go back to (l) and how many times to go back (r) instead of the macro: "s=m12/d0;m15/d250;l0/r3/d500".
    // Neither colons nor commas can be used, as they separate the parts of stored lines and the arguments of commands.
    //
    // Unknown step fields are skipped like other fields, but steps that are neither macros nor loops make the whole
    // sequence invalid, as skipping them would change what it does and which steps its loops go back to.
    class SequenceMacroDataSerializer final
        : public MacroDataStorageSerializer<SequenceMacroData> {
    public:
        bool handles(const MacroType type) override {
            return type == SEQUENCE;
        };

        std::string_view serialize(const SequenceMacroData& data, tfw::utils::allocations::Arena& arena) override {
            std::string steps;
            for (const auto& step : data.steps) {
                steps += steps.empty() ? "s=" : ";";
                if (step.isLoop()) {
                    steps += 'l';
                    steps += std::to_string(step.loopTarget);
                    steps += "/r";
                    steps += std::to_string(step.loopRepeats);
                } else {
                    steps += 'm';
                    steps += std::to_string(step.macroId);
                }
                steps += "/d";
                steps += std::to_string(step.delayMs);
            }

            return tfw::utils::allocations::arena::strings::sprintf(
                arena,
                "%.*s",
                static_cast<int>(steps.length()),
                steps.data()
            );
        }

        std::shared_ptr<SequenceMacroData> deserialize(
            const uint16_t macroId,
            const std::span<const std::string_view>& parts,
            tfw::utils::allocations::Arena& arena
        ) override {
            const auto fields = Fields::parse(parts);
            if (!fields) {
                return nullptr;
            }

            std::vector<SequenceStep> steps;
            std::string_view remaining = fields->get("s").value_or("");
            while (!remaining.empty()) {
                const auto separator = remaining.find(';');
                const auto step = parseStep(remaining.substr(0, separator), steps.size());
                if (!step.has_value()) {
                    return nullptr;
                }
                steps.push_back(*step);
                remaining = separator == std::string_view::npos ? "" : remaining.substr(separator + 1);
            }

            // Sequences without steps can't be run as intended
            if (steps.empty() || steps.size() > MaxSequenceSteps) {
                return nullptr;
            }

            const tfw::utils::allocations::ArenaAllocator<SequenceMacroData> allocator(arena);

            return std::allocate_shared<SequenceMacroData>(allocator, macroId, std::move(steps));
        }

    private:
        static std::optional<SequenceStep> parseStep(std::string_view remaining, const size_t index) {
            std::optional<uint32_t> macroId;
            std::optional<uint32_t> delay;
            std::optional<uint32_t> target;
            std::optional<uint32_t> repeats;

            while (true) {
                const auto separator = remaining.find('/');
                const auto field = remaining.substr(0, separator);
                if (field.length() < 2) {
                    return std::nullopt;
                }

                const auto value = tfw::utils::strings::atou32(field.substr(1));
                switch (field.front()) {
                    case 'm': macroId = value; break;
                    case 'd': delay = value; break;
                    case 'l': target = value; break;
                    case 'r': repeats = value; break;
                    default: break;
                }

                if (separator == std::string_view::npos) {
                    break;
                }
                remaining = remaining.substr(separator + 1);
            }

            const auto delayMs = std::min(delay.value_or(0), MaxSequenceStepDelayMs);

            if (macroId.has_value() && !target.has_value()) {
                return SequenceStep{
                    .macroId = static_cast<uint16_t>(*macroId),
                    .delayMs = delayMs,
                };
            }

            if (target.has_value() && !macroId.has_value()) {
                // Loops can only go back, and ones that don't go back at all would be steps that do nothing
                const auto repeatCount = repeats.value_or(0);
                if (*target >= index || repeatCount == 0) {
                    return std::nullopt;
                }
                return SequenceStep{
                    .macroId = 0,
                    .delayMs = delayMs,
                    .loopTarget = static_cast<uint8_t>(*target),
                    .loopRepeats = static_cast<uint16_t>(std::min<uint32_t>(repeatCount, MaxSequenceLoopRepeats)),
                };
            }

            return std::nullopt;
        }
    };
}
