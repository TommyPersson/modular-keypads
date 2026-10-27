#include "UuidV4Generator.h"

#include <array>

using namespace mkp::components::generators;

void UuidV4Generator::write(tfw::hal::random::RandomSource& randomSource, char* destination) const {
    std::array<uint8_t, 16> bytes{};
    randomSource.fill(bytes.data(), bytes.size());

    // The version in the high nibble of byte 6, and the variant (binary 10) in the high bits of byte 8
    bytes[6] = (bytes[6] & 0x0f) | 0x40;
    bytes[8] = (bytes[8] & 0x3f) | 0x80;

    constexpr auto hexDigits = "0123456789abcdef";

    for (size_t i = 0; i < bytes.size(); i++) {
        if (i == 4 || i == 6 || i == 8 || i == 10) {
            *destination++ = '-';
        }
        *destination++ = hexDigits[bytes[i] >> 4];
        *destination++ = hexDigits[bytes[i] & 0x0f];
    }
}
