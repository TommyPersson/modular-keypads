#pragma once

#include <algorithm>
#include <cstdint>
#include <optional>
#include <span>
#include <string_view>

#include <tfw/utils/strings.h>

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

    inline int8_t clampToInt8(const int value) {
        return static_cast<int8_t>(std::clamp(value, -127, 127));
    }

    inline int16_t clampToInt16(const int value) {
        return static_cast<int16_t>(std::clamp(value, -32767, 32767));
    }
}
