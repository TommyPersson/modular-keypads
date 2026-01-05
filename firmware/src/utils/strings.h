#pragma once

#include <cstring>
#include <string>

namespace utils::strings {
    inline int atol(const std::string_view& string, const int base = 10) {
        char temp[string.length() + 1];
        temp[string.length()] = '\0';
        std::strncpy(&temp[0], string.data(), string.length());

        return ::strtol(temp, nullptr, base);
    }

    inline uint64_t atou64(const std::string_view& string, const int base = 10) {
        char temp[string.length() + 1];
        temp[string.length()] = '\0';
        std::strncpy(&temp[0], string.data(), string.length());

        return ::strtoull(temp, nullptr, base);
    }

    inline uint16_t atou16(const std::string_view& string, const int base = 10) {
        char temp[string.length() + 1];
        temp[string.length()] = '\0';
        std::strncpy(&temp[0], string.data(), string.length());

        return ::strtoul(temp, nullptr, base);
    }

    inline size_t uriDecode(const std::string_view& text, char* buffer, size_t length) {
        uint32_t bi = 0;

        for (uint32_t i = 0; i < text.length() && bi < length; ++i, ++bi) {
            char c = text[i];
            if (c == '%') {
                c = atol(text.substr(i + 1, 2), 16);
                i += 2;
            }
            buffer[bi] = c;
        }

        buffer[bi] = '\0';

        return bi;
    }
}
