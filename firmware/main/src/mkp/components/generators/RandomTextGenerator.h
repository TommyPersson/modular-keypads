#pragma once

#include <cstddef>
#include <string_view>

#include <tfw/hal/random.h>
#include <tfw/utils/allocations/Arena.h>

#include "mkp/components/macros/Macro.h"

namespace mkp::components::generators {
    // Makes random text of a format, such as an ID, from the random bytes of a source. Subclasses only write the text
    // of their format, and the arena and quotes are handled here for all of them.
    class RandomTextGenerator {
    public:
        virtual ~RandomTextGenerator() = default;

        // The generator of the format, or null for unknown formats, such as ones added in later firmware. The
        // generators live as long as the program.
        static const RandomTextGenerator* forFormat(macros::RandomTextFormat format);

        // Random text, optionally wrapped in double quotes. Written into the arena rather than allocated, and empty
        // when the arena is too small.
        std::string_view generate(
            bool quoted,
            tfw::hal::random::RandomSource& randomSource,
            tfw::utils::allocations::Arena& arena
        ) const;
        
    protected:
        // Number of characters written by write()
        virtual size_t length() const = 0;

        // Writes length() characters to the destination
        virtual void write(tfw::hal::random::RandomSource& randomSource, char* destination) const = 0;
    };
}
