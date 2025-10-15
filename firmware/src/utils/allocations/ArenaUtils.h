#pragma once

#include <span>
#include <string_view>

#include "Arena.h"

namespace arena::strings {
    vector<std::string_view> split(
        const std::string_view& str,
        char delimiter,
        const ArenaAllocator<std::string_view>& allocator,
        int initialCapacity = 4
    );

    std::string join(
        const std::span<const std::string>& strings,
        const std::string& delimiter,
        Arena& arena
        );

    std::string join(
        const std::span<const std::string_view>& strings,
        const std::string& delimiter,
        Arena& arena
    );

    std::string sprintf(Arena& arena, const char* format, ...) __attribute__((format(printf, 2, 3)));
}