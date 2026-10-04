#pragma once

#include <stdint.h>

/**
 * CRC-8/AUTOSAR: polynomial 0x2F, init 0xFF, no reflection, final XOR 0xFF.
 * Check value: compute("123456789") == 0xDF.
 *
 * The non-zero init and final XOR make all-0x00 and all-0xFF data (e.g. a stuck line) fail the check.
 */
namespace smc::utils::crc8 {
    inline constexpr uint8_t POLYNOMIAL = 0x2F;
    inline constexpr uint8_t INIT = 0xFF;
    inline constexpr uint8_t XOR_OUT = 0xFF;

    struct Table {
        uint8_t values[256];

        constexpr Table() : values() {
            for (int i = 0; i < 256; i++) {
                uint8_t crc = i;
                for (int bit = 0; bit < 8; bit++) {
                    crc = crc & 0x80 ? (uint8_t)(crc << 1) ^ POLYNOMIAL : (uint8_t)(crc << 1);
                }
                values[i] = crc;
            }
        }
    };

    // Generated at compile time, stored in flash
    inline constexpr Table TABLE{};

    // Start with INIT, feed each byte through update(), finish with finalize()
    inline uint8_t update(const uint8_t crc, const uint8_t byte) {
        return TABLE.values[crc ^ byte];
    }

    inline uint8_t finalize(const uint8_t crc) {
        return crc ^ XOR_OUT;
    }

    inline uint8_t compute(const uint8_t* data, const uint8_t length) {
        uint8_t crc = INIT;
        for (uint8_t i = 0; i < length; i++) {
            crc = update(crc, data[i]);
        }
        return finalize(crc);
    }
}
