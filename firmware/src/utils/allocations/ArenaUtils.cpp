#include "ArenaUtils.h"

#include "Arena.h"

namespace arena::strings {
    vector<std::string_view> split(
        const std::string_view& str,
        const char delimiter,
        const ArenaAllocator<std::string_view>& allocator,
        const int initialCapacity
        ) {

        vector parts(allocator);
        parts.reserve(initialCapacity);

        int nextPartOffset = 0;
        for (int i = 0; i < str.length(); i++) {
            const auto c = str[i];

            if (c == delimiter) {
                const auto nextPartLength = i - nextPartOffset;
                const auto part = str.substr(nextPartOffset, nextPartLength);
                parts.emplace_back(part);

                nextPartOffset = i + 1;
            }
        }

        const auto finalPartLength = str.length() - nextPartOffset;
        const auto finalPart = str.substr(nextPartOffset, finalPartLength);
        parts.emplace_back(finalPart);

        return parts;
    }
}
