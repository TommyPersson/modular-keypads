#pragma once

#include <cstring>
#include <string>

namespace tfw::utils::strings {
    inline int atol(const std::string_view& string, const int base = 10) {
        char temp[string.length() + 1];
        temp[string.length()] = '\0';
        std::strncpy(&temp[0], string.data(), string.length());

        return ::strtol(temp, nullptr, base);
    }

    inline uint8_t atou8(const std::string_view& string, const int base = 10) {
        char temp[string.length() + 1];
        temp[string.length()] = '\0';
        std::strncpy(&temp[0], string.data(), string.length());

        return ::strtoul(temp, nullptr, base);
    }

    inline uint16_t atou16(const std::string_view& string, const int base = 10) {
        char temp[string.length() + 1];
        temp[string.length()] = '\0';
        std::strncpy(&temp[0], string.data(), string.length());

        return ::strtoul(temp, nullptr, base);
    }

    inline uint32_t atou32(const std::string_view& string, const int base = 10) {
        char temp[string.length() + 1];
        temp[string.length()] = '\0';
        std::strncpy(&temp[0], string.data(), string.length());

        return ::strtoul(temp, nullptr, base);
    }

    inline uint64_t atou64(const std::string_view& string, const int base = 10) {
        char temp[string.length() + 1];
        temp[string.length()] = '\0';
        std::strncpy(&temp[0], string.data(), string.length());

        return ::strtoull(temp, nullptr, base);
    }

    inline size_t uriDecode(const std::string_view& text, char* buffer, size_t length) {
        if (length == 0) {
            return 0;
        }

        uint32_t bi = 0;
        size_t max_chars = length - 1;  // Reserve space for null terminator

        for (uint32_t i = 0; i < text.length() && bi < max_chars; ++i, ++bi) {
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
