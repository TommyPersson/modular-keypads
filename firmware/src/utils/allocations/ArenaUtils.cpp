#include "ArenaUtils.h"

#include <HardwareSerial.h>
#include <pstl/execution_defs.h>

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

std::string arena::strings::join(
    const std::span<const std::string_view>& strings,
    const std::string& delimiter,
    Arena& arena
) {
    const ArenaAllocator<char> allocator(arena);

    ostringstream ss(std::ios_base::out, allocator);

    for (int i = 0; i < strings.size(); i++) {
        ss << strings[i];
        if (i != strings.size() - 1) {
            ss << delimiter;
        }
    }

    return std::string(ss.str());
}

std::string arena::strings::join(
    const std::span<const std::string>& strings,
    const std::string& delimiter,
    Arena& arena
) {
    const ArenaAllocator<char> allocator(arena);

    ostringstream ss(std::ios_base::out, allocator);

    for (int i = 0; i < strings.size(); i++) {
        ss << strings[i];
        if (i != strings.size() - 1) {
            ss << delimiter;
        }
    }

    return std::string(ss.str());
}

std::string_view arena::strings::sprintf(Arena& arena, const char* format, ...) {
    va_list args;
    va_start(args, format);

    const auto bufferSize = static_cast<size_t>(vsnprintf(nullptr, 0, format, args) + 1);
    const auto buffer = reinterpret_cast<char*>(arena.allocate(bufferSize));

    std::vsnprintf(buffer, bufferSize, format, args);

    va_end(args);

    // ReSharper disable once CppDFALocalValueEscapesFunction
    return {buffer, bufferSize};
}
