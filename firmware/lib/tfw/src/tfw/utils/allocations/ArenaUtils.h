#pragma once

#include <format>
#include <span>
#include <string_view>

#include "Arena.h"

namespace tfw::utils::allocations::arena::strings {
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

    std::string_view sprintf(Arena& arena, const char* format, ...) __attribute__((format(printf, 2, 3)));

    /// Modern C++20 overload using std::format (type-safe, no need for format specifiers)
    template<typename... Args>
    std::string_view sprintf2(Arena& arena, std::format_string<Args...> format, Args&&... args) {
        const auto formatted = std::format(format, std::forward<Args>(args)...);
        const auto buffer = reinterpret_cast<char*>(arena.allocate(formatted.size() + 1));
        std::memcpy(buffer, formatted.data(), formatted.size());
        buffer[formatted.size()] = '\0';
        return {buffer, formatted.size()};
    }
}