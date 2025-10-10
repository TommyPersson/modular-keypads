#pragma once

#include <string_view>

#include "Arena.h"

namespace arena::strings {
    vector<std::string_view> split(
        const std::string_view& str,
        char delimiter,
        const ArenaAllocator<std::string_view>& allocator,
        int initialCapacity = 4
    );
}
