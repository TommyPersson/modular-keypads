#include "MacroDataSerializers.h"

#include <algorithm>

#include "mkp/components/settings/Settings.h"

namespace mkp::components::macros {
    // Macro data with any other number of parts can't be parsed, and is ignored
    static bool hasParts(const std::span<const std::string_view>& parts, const size_t count) {
        return parts.size() == count;
    }

    static int8_t clampToInt8(const int value) {
        return static_cast<int8_t>(std::clamp(value, -127, 127));
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
                "0x%04x",
                data.usageId
            );
        }

        std::shared_ptr<ConsumerControlMacroData> deserialize(
            const uint16_t macroId,
            const std::span<const std::string_view>& parts,
            tfw::utils::allocations::Arena& arena
        ) override {
            if (!hasParts(parts, 1)) {
                return nullptr;
            }

            const tfw::utils::allocations::ArenaAllocator<ConsumerControlMacroData> allocator(arena);

            const auto usageIdPart = parts[0];

            return std::allocate_shared<ConsumerControlMacroData>(
                allocator,
                macroId,
                tfw::utils::strings::atou16(usageIdPart, 16)
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
                "0x%02x:0x%02x:%d:%u",
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
            if (!hasParts(parts, 4)) {
                return nullptr;
            }

            const tfw::utils::allocations::ArenaAllocator<ShortcutMacroData> allocator(arena);

            const auto modifiersPart = parts[0];
            const auto hidCodePart = parts[1];
            const auto holdModifiersPart = parts[2];
            const auto holdTimeMsPart = parts[3];

            return std::allocate_shared<ShortcutMacroData>(
                allocator,
                macroId,
                tfw::utils::strings::atol(modifiersPart, 16),
                tfw::utils::strings::atol(hidCodePart, 16),
                tfw::utils::strings::atol(holdModifiersPart) != 0,
                std::min(tfw::utils::strings::atou32(holdTimeMsPart), settings::ModifierHoldMs.maxValue)
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
                "0x%02x",
                data.code
            );
        }

        std::shared_ptr<SystemControlMacroData> deserialize(
            const uint16_t macroId,
            const std::span<const std::string_view>& parts,
            tfw::utils::allocations::Arena& arena
        ) override {
            if (!hasParts(parts, 1)) {
                return nullptr;
            }

            const tfw::utils::allocations::ArenaAllocator<SystemControlMacroData> allocator(arena);

            const auto codePart = parts[0];

            return std::allocate_shared<SystemControlMacroData>(
                allocator,
                macroId,
                tfw::utils::strings::atol(codePart, 16)
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
                "%.*s",
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
            if (!hasParts(parts, 1)) {
                return nullptr;
            }

            const tfw::utils::allocations::ArenaAllocator<TextMacroData> allocator(arena);

            const auto textPart = parts[0];

            return std::allocate_shared<TextMacroData>(
                allocator,
                macroId,
                textPart
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
                "0x%02x:%d:%d",
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
            if (!hasParts(parts, 3)) {
                return nullptr;
            }

            const tfw::utils::allocations::ArenaAllocator<MouseScrollMacroData> allocator(arena);

            return std::allocate_shared<MouseScrollMacroData>(
                allocator,
                macroId,
                tfw::utils::strings::atou8(parts[0], 16),
                clampToInt8(tfw::utils::strings::atol(parts[1])),
                clampToInt8(tfw::utils::strings::atol(parts[2]))
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
                "0x%02x:0x%02x",
                data.modifiers,
                data.buttons
            );
        }

        std::shared_ptr<MouseButtonMacroData> deserialize(
            const uint16_t macroId,
            const std::span<const std::string_view>& parts,
            tfw::utils::allocations::Arena& arena
        ) override {
            if (!hasParts(parts, 2)) {
                return nullptr;
            }

            const tfw::utils::allocations::ArenaAllocator<MouseButtonMacroData> allocator(arena);

            return std::allocate_shared<MouseButtonMacroData>(
                allocator,
                macroId,
                tfw::utils::strings::atou8(parts[0], 16),
                tfw::utils::strings::atou8(parts[1], 16)
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
                "0x%02x:%d:%d",
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
            if (!hasParts(parts, 3)) {
                return nullptr;
            }

            const tfw::utils::allocations::ArenaAllocator<MouseMovementMacroData> allocator(arena);

            return std::allocate_shared<MouseMovementMacroData>(
                allocator,
                macroId,
                tfw::utils::strings::atou8(parts[0], 16),
                clampToInt8(tfw::utils::strings::atol(parts[1])),
                clampToInt8(tfw::utils::strings::atol(parts[2]))
            );
        }
    };

    // The steps as one part, so that long sequences don't split into more parts than a line can hold, as the macro ID
    // and delay of each step separated by semicolons: "12;0;15;250". Neither colons nor commas can be used, as they
    // separate the parts of stored lines and the arguments of commands.
    class SequenceDataSerializer final
        : public MacroDataStorageSerializer<SequenceMacroData> {
    public:
        bool handles(const MacroType type) override {
            return type == SEQUENCE;
        };

        std::string_view serialize(const SequenceMacroData& data, tfw::utils::allocations::Arena& arena) override {
            std::string steps;
            for (const auto& step : data.steps) {
                if (!steps.empty()) {
                    steps += ';';
                }
                steps += std::to_string(step.macroId);
                steps += ';';
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
            if (!hasParts(parts, 1)) {
                return nullptr;
            }

            std::vector<uint32_t> numbers;
            std::string_view remaining = parts[0];
            while (!remaining.empty()) {
                const auto separator = remaining.find(';');
                const auto number = remaining.substr(0, separator);
                if (number.empty()) {
                    return nullptr;
                }
                numbers.push_back(tfw::utils::strings::atou32(number));
                remaining = separator == std::string_view::npos ? "" : remaining.substr(separator + 1);
            }

            // Sequences without steps, or with a step missing its delay, can't be run as intended
            if (numbers.empty() || numbers.size() % 2 != 0 || numbers.size() / 2 > MaxSequenceSteps) {
                return nullptr;
            }

            std::vector<SequenceStep> steps;
            steps.reserve(numbers.size() / 2);
            for (size_t i = 0; i < numbers.size(); i += 2) {
                steps.push_back(SequenceStep{
                    .macroId = static_cast<uint16_t>(numbers[i]),
                    .delayMs = std::min(numbers[i + 1], MaxSequenceStepDelayMs),
                });
            }

            const tfw::utils::allocations::ArenaAllocator<SequenceMacroData> allocator(arena);

            return std::allocate_shared<SequenceMacroData>(allocator, macroId, std::move(steps));
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
    };
}
