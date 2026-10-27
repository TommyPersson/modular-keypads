#pragma once

#include <cstddef>
#include <cstdint>

namespace tfw::hal::random {
    // Source of random bytes, such as for generated IDs and keys. Allows tests to use predictable bytes.
    class RandomSource {
    public:
        virtual ~RandomSource() = default;

        virtual void fill(uint8_t* buffer, size_t length) = 0;
    };
}
