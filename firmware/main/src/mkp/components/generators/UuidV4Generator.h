#pragma once

#include "RandomTextGenerator.h"

namespace mkp::components::generators {
    // A version 4 (random) UUID as in RFC 9562, lowercase with hyphens: 0f8fad5b-d9cb-469f-a165-70867728950e
    class UuidV4Generator final : public RandomTextGenerator {
    protected:
        size_t length() const override { return 36; }

        void write(tfw::hal::random::RandomSource& randomSource, char* destination) const override;
    };
}
