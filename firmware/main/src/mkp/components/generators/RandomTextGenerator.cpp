#include "RandomTextGenerator.h"

#include "UuidV4Generator.h"

using namespace mkp::components::generators;
using mkp::components::macros::RandomTextFormat;

namespace {
    const UuidV4Generator uuidV4;
}

const RandomTextGenerator* RandomTextGenerator::forFormat(const RandomTextFormat format) {
    switch (format) {
    case RandomTextFormat::UUID_V4:
        return &uuidV4;
    default:
        return nullptr;
    }
}

std::string_view RandomTextGenerator::generate(
    const bool quoted,
    tfw::hal::random::RandomSource& randomSource,
    tfw::utils::allocations::Arena& arena
) const {
    const size_t quoteLength = quoted ? 1 : 0;
    const auto totalLength = length() + 2 * quoteLength;
    const auto buffer = reinterpret_cast<char*>(arena.allocate(totalLength));
    if (buffer == nullptr) {
        return {};
    }

    write(randomSource, buffer + quoteLength);

    if (quoted) {
        buffer[0] = '"';
        buffer[totalLength - 1] = '"';
    }

    return {buffer, totalLength};
}
