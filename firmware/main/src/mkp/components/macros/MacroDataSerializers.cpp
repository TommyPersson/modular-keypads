#include "MacroDataSerializers.h"

#include <algorithm>
#include <optional>

#include "mkp/components/generators/RandomTextGenerator.h"
#include "mkp/components/settings/Settings.h"

namespace mkp::components::macros {
    // The data parts are "key=value" fields, so that fields can be added and removed without breaking stored data or
    // other firmware versions: keys that aren't known are skipped, and missing keys take their defaults.
    class Fields {
    public:
        static std::optional<Fields> parse(const std::span<const std::string_view>& parts) {
            // Parts without keys can't be told apart from each other
            const bool keyed = std::ranges::all_of(
                parts,
                [](const std::string_view& part) { return part.find('=') != std::string_view::npos; }
            );

            if (!keyed) {
                return std::nullopt;
            }

            return Fields(parts);
        }

        std::optional<std::string_view> get(const std::string_view& key) const {
            for (const auto& part : parts) {
                const auto separator = part.find('=');
                if (part.substr(0, separator) == key) {
                    return part.substr(separator + 1);
                }
            }

            return std::nullopt;
        }

        int number(const std::string_view& key, const int defaultValue, const int base = 10) const {
            const auto value = get(key);
            return value.has_value() ? tfw::utils::strings::atol(*value, base) : defaultValue;
        }

        uint32_t unsignedNumber(const std::string_view& key, const uint32_t defaultValue, const int base = 10) const {
            const auto value = get(key);
            return value.has_value() ? tfw::utils::strings::atou32(*value, base) : defaultValue;
        }

        bool flag(const std::string_view& key, const bool defaultValue = false) const {
            const auto value = get(key);
            return value.has_value() ? tfw::utils::strings::atol(*value) != 0 : defaultValue;
        }

    private:
        explicit Fields(const std::span<const std::string_view>& parts) : parts(parts) {
        }

        std::span<const std::string_view> parts;
    };

    static int8_t clampToInt8(const int value) {
        return static_cast<int8_t>(std::clamp(value, -127, 127));
    }

    static int16_t clampToInt16(const int value) {
        return static_cast<int16_t>(std::clamp(value, -32767, 32767));
    }

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

    class ShortcutMacroDataSerializer final
        : public MacroDataStorageSerializer<ShortcutMacroData> {
    public:
        bool handles(const MacroType type) override {
            return type == SHORTCUT;
        };

        std::string_view serialize(const ShortcutMacroData& data, tfw::utils::allocations::Arena& arena) override {
            return tfw::utils::allocations::arena::strings::sprintf(
                arena,
                "m=0x%02x:k=0x%02x:h=%d:t=%u",
                data.modifiers,
                data.hidKeyCode,
                data.holdModifiers ? 1 : 0,
                static_cast<unsigned int>(data.holdTimeMs)
            );
        }

        std::shared_ptr<ShortcutMacroData> deserialize(
            const uint16_t macroId,
            const std::span<const std::string_view>& parts,
            tfw::utils::allocations::Arena& arena
        ) override {
            const auto fields = Fields::parse(parts);
            if (!fields) {
                return nullptr;
            }

            const tfw::utils::allocations::ArenaAllocator<ShortcutMacroData> allocator(arena);

            return std::allocate_shared<ShortcutMacroData>(
                allocator,
                macroId,
                fields->number("m", 0, 16),
                fields->number("k", 0, 16),
                fields->flag("h"),
                std::min(fields->unsignedNumber("t", 0), settings::ModifierHoldMs.maxValue)
            );
        }
    };

    class SystemControlDataSerializer final
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

    class TextDataSerializer final
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

    class MouseScrollDataSerializer final
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

    class MouseButtonDataSerializer final
        : public MacroDataStorageSerializer<MouseButtonMacroData> {
    public:
        bool handles(const MacroType type) override {
            return type == MOUSE_BUTTON;
        };

        std::string_view serialize(const MouseButtonMacroData& data, tfw::utils::allocations::Arena& arena) override {
            return tfw::utils::allocations::arena::strings::sprintf(
                arena,
                "m=0x%02x:b=0x%02x",
                data.modifiers,
                data.buttons
            );
        }

        std::shared_ptr<MouseButtonMacroData> deserialize(
            const uint16_t macroId,
            const std::span<const std::string_view>& parts,
            tfw::utils::allocations::Arena& arena
        ) override {
            const auto fields = Fields::parse(parts);
            if (!fields) {
                return nullptr;
            }

            const tfw::utils::allocations::ArenaAllocator<MouseButtonMacroData> allocator(arena);

            return std::allocate_shared<MouseButtonMacroData>(
                allocator,
                macroId,
                static_cast<uint8_t>(fields->unsignedNumber("m", 0, 16)),
                static_cast<uint8_t>(fields->unsignedNumber("b", 0, 16))
            );
        }
    };

    class MouseMovementDataSerializer final
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

    class MidiNoteDataSerializer final
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

    class MidiRelativeControlChangeDataSerializer final
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

    class MidiAbsoluteControlChangeDataSerializer final
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

    class RandomTextDataSerializer final
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

    // The steps are one field, separated by semicolons, with the fields of each step separated by slashes. A step
    // field is a letter followed by a number: the macro to run (m) and the delay (d), or for loops, the index of the
    // step to go back to (l) and how many times to go back (r) instead of the macro: "s=m12/d0;m15/d250;l0/r3/d500".
    // Neither colons nor commas can be used, as they separate the parts of stored lines and the arguments of commands.
    //
    // Unknown step fields are skipped like other fields, but steps that are neither macros nor loops make the whole
    // sequence invalid, as skipping them would change what it does and which steps its loops go back to.
    class SequenceDataSerializer final
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

    std::vector<void*> macroDataSerializers{
        new ShortcutMacroDataSerializer{},
        new ConsumerControlMacroDataSerializer{},
        new SystemControlDataSerializer{},
        new TextDataSerializer{},
        new MouseScrollDataSerializer{},
        new MouseButtonDataSerializer{},
        new MouseMovementDataSerializer{},
        new SequenceDataSerializer{},
        new MidiNoteDataSerializer{},
        new MidiRelativeControlChangeDataSerializer{},
        new MidiAbsoluteControlChangeDataSerializer{},
        new RandomTextDataSerializer{},
    };
}
