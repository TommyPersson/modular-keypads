#pragma once

#include <string>

namespace utils::strings {
    inline int atol(const std::string_view& string, const int base = 10) {
        return ::strtol(string.data(), nullptr, base);
    }

    inline uint64_t atou64(const std::string_view& string, const int base = 10) {
        return ::strtoull(string.data(), nullptr, base);
    }

    inline int atol(const std::string_view& string, int& intLength) {
        char* endPtr;
        long result = ::strtol(string.data(), &endPtr, 10);
        auto offset = endPtr - string.data();
        intLength = offset;
        return result;
    }
}
