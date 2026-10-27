#include <gtest/gtest.h>

#include "mkp/components/generators/RandomTextGenerator.cpp"
#include "mkp/components/generators/UuidV4Generator.cpp"
#include "../../../lib/tfw/hal/StubRandomSource.h"

using namespace mkp::components::generators;
using mkp::components::macros::RandomTextFormat;
using tfw::hal::random::test::StubRandomSource;
using tfw::utils::allocations::Arena;

namespace {
    // A generator that writes its length in x characters, so that the shared handling can be tested on its own
    class FixedGenerator final : public RandomTextGenerator {
    public:
        explicit FixedGenerator(const size_t length) : fixedLength(length) {
        }

    protected:
        size_t length() const override { return fixedLength; }

        void write(tfw::hal::random::RandomSource& randomSource, char* destination) const override {
            for (size_t i = 0; i < fixedLength; i++) {
                destination[i] = 'x';
            }
        }

    private:
        size_t fixedLength;
    };

    std::string_view generate(const RandomTextGenerator& generator, StubRandomSource& source, const bool quoted = false) {
        static Arena arena{256};
        arena.reset();
        return generator.generate(quoted, source, arena);
    }
}

// ============================================================================
// Lookup
// ============================================================================

TEST(RandomTextGeneratorsTest, FindsGeneratorOfKnownFormat) {
    EXPECT_NE(dynamic_cast<const UuidV4Generator*>(RandomTextGenerator::forFormat(RandomTextFormat::UUID_V4)), nullptr);
}

TEST(RandomTextGeneratorsTest, FindsNoGeneratorOfUnknownFormat) {
    EXPECT_EQ(RandomTextGenerator::forFormat(static_cast<RandomTextFormat>(0)), nullptr);
    EXPECT_EQ(RandomTextGenerator::forFormat(static_cast<RandomTextFormat>(99)), nullptr);
}

// ============================================================================
// Generating into an arena
// ============================================================================

TEST(RandomTextGeneratorsTest, TextIsWrittenIntoTheArena) {
    StubRandomSource source{{0x00}};
    Arena arena{64};

    const auto text = FixedGenerator{3}.generate(false, source, arena);

    EXPECT_EQ(text, "xxx");
    EXPECT_EQ(text.data(), reinterpret_cast<const char*>(arena.getBuffer()));
}

TEST(RandomTextGeneratorsTest, QuotedTextIsWrappedInDoubleQuotes) {
    StubRandomSource source{{0x00}};

    EXPECT_EQ(generate(FixedGenerator{3}, source, true), "\"xxx\"");
}

TEST(RandomTextGeneratorsTest, TextIsEmptyWhenTheArenaIsTooSmall) {
    StubRandomSource source{{0x00}};
    Arena arena{4};

    EXPECT_EQ(FixedGenerator{3}.generate(true, source, arena), "");
}

// ============================================================================
// UUID v4
// ============================================================================

TEST(RandomTextGeneratorsTest, UuidV4HasVersionAndVariantBits) {
    StubRandomSource source{{0xff}};

    EXPECT_EQ(generate(UuidV4Generator{}, source), "ffffffff-ffff-4fff-bfff-ffffffffffff");
}

TEST(RandomTextGeneratorsTest, UuidV4IsFormattedInHyphenatedGroups) {
    StubRandomSource source{{
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff
    }};

    EXPECT_EQ(generate(UuidV4Generator{}, source), "00112233-4455-4677-8899-aabbccddeeff");
}

TEST(RandomTextGeneratorsTest, QuotedUuidV4IsWrittenBetweenTheQuotes) {
    StubRandomSource source{{0x00}};

    EXPECT_EQ(generate(UuidV4Generator{}, source, true), "\"00000000-0000-4000-8000-000000000000\"");
}
