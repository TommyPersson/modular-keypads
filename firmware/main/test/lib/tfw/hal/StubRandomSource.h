#pragma once

#include <cstdint>
#include <vector>

#include <tfw/hal/random.h>

namespace tfw::hal::random::test {
    // Fills buffers with the given bytes, repeated as needed, so that tests get predictable values
    class StubRandomSource final : public RandomSource {
    public:
        explicit StubRandomSource(std::vector<uint8_t> bytes) : bytes(std::move(bytes)) {
        }

        void fill(uint8_t* buffer, const size_t length) override {
            for (size_t i = 0; i < length; i++) {
                buffer[i] = bytes[next++ % bytes.size()];
            }
        }

    private:
        std::vector<uint8_t> bytes;
        size_t next = 0;
    };
}
