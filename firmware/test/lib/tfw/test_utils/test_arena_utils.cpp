#include <gtest/gtest.h>

#include "tfw/utils/allocations.h"

using namespace tfw::utils::allocations;
using namespace tfw::utils::allocations::arena;
using namespace tfw::utils::allocations::arena::strings;

class ArenaUtilsTest : public ::testing::Test {
protected:
    Arena arena{8192};
};

// ============================================================================
// split() Tests
// ============================================================================

TEST_F(ArenaUtilsTest, SplitSimple) {
    const std::string_view input = "hello,world,test";
    const ArenaAllocator<std::string_view> allocator(arena);

    auto parts = split(input, ',', allocator);

    EXPECT_EQ(parts.size(), 3);
    EXPECT_EQ(parts[0], "hello");
    EXPECT_EQ(parts[1], "world");
    EXPECT_EQ(parts[2], "test");
}

TEST_F(ArenaUtilsTest, SplitSinglePart) {
    const std::string_view input = "hello";
    const ArenaAllocator<std::string_view> allocator(arena);

    auto parts = split(input, ',', allocator);

    EXPECT_EQ(parts.size(), 1);
    EXPECT_EQ(parts[0], "hello");
}

TEST_F(ArenaUtilsTest, SplitEmpty) {
    const std::string_view input = "";
    const ArenaAllocator<std::string_view> allocator(arena);

    auto parts = split(input, ',', allocator);

    EXPECT_EQ(parts.size(), 1);
    EXPECT_EQ(parts[0], "");
}

TEST_F(ArenaUtilsTest, SplitWithConsecutiveDelimiters) {
    const std::string_view input = "hello,,world";
    const ArenaAllocator<std::string_view> allocator(arena);

    auto parts = split(input, ',', allocator);

    EXPECT_EQ(parts.size(), 3);
    EXPECT_EQ(parts[0], "hello");
    EXPECT_EQ(parts[1], "");
    EXPECT_EQ(parts[2], "world");
}

TEST_F(ArenaUtilsTest, SplitTrailingDelimiter) {
    const std::string_view input = "hello,world,";
    const ArenaAllocator<std::string_view> allocator(arena);

    auto parts = split(input, ',', allocator);

    EXPECT_EQ(parts.size(), 3);
    EXPECT_EQ(parts[0], "hello");
    EXPECT_EQ(parts[1], "world");
    EXPECT_EQ(parts[2], "");
}

TEST_F(ArenaUtilsTest, SplitLeadingDelimiter) {
    const std::string_view input = ",hello,world";
    const ArenaAllocator<std::string_view> allocator(arena);

    auto parts = split(input, ',', allocator);

    EXPECT_EQ(parts.size(), 3);
    EXPECT_EQ(parts[0], "");
    EXPECT_EQ(parts[1], "hello");
    EXPECT_EQ(parts[2], "world");
}

TEST_F(ArenaUtilsTest, SplitDifferentDelimiter) {
    const std::string_view input = "hello;world;test";
    const ArenaAllocator<std::string_view> allocator(arena);

    auto parts = split(input, ';', allocator);

    EXPECT_EQ(parts.size(), 3);
    EXPECT_EQ(parts[0], "hello");
    EXPECT_EQ(parts[1], "world");
    EXPECT_EQ(parts[2], "test");
}

TEST_F(ArenaUtilsTest, SplitWithSpaces) {
    const std::string_view input = "hello world,foo bar";
    const ArenaAllocator<std::string_view> allocator(arena);

    auto parts = split(input, ',', allocator);

    EXPECT_EQ(parts.size(), 2);
    EXPECT_EQ(parts[0], "hello world");
    EXPECT_EQ(parts[1], "foo bar");
}

TEST_F(ArenaUtilsTest, SplitCustomInitialCapacity) {
    const std::string_view input = "a,b,c,d,e";
    const ArenaAllocator<std::string_view> allocator(arena);

    auto parts = split(input, ',', allocator, 10);

    EXPECT_EQ(parts.size(), 5);
    EXPECT_EQ(parts[0], "a");
    EXPECT_EQ(parts[4], "e");
}

// ============================================================================
// join() Tests with string_view
// ============================================================================

TEST_F(ArenaUtilsTest, JoinStringViewSimple) {
    const std::string_view parts[] = {"hello", "world", "test"};
    const auto result = join(std::span(parts), ",", arena);

    EXPECT_EQ(result, "hello,world,test");
}

TEST_F(ArenaUtilsTest, JoinStringViewSinglePart) {
    const std::string_view parts[] = {"hello"};
    const auto result = join(std::span(parts), ",", arena);

    EXPECT_EQ(result, "hello");
}

TEST_F(ArenaUtilsTest, JoinStringViewEmpty) {
    std::vector<std::string_view> parts;
    const auto result = join(std::span(parts), ",", arena);

    EXPECT_EQ(result, "");
}

TEST_F(ArenaUtilsTest, JoinStringViewWithEmptyParts) {
    const std::string_view parts[] = {"hello", "", "world"};
    const auto result = join(std::span(parts), ",", arena);

    EXPECT_EQ(result, "hello,,world");
}

TEST_F(ArenaUtilsTest, JoinStringViewDifferentDelimiter) {
    const std::string_view parts[] = {"hello", "world", "test"};
    const auto result = join(std::span(parts), ";", arena);

    EXPECT_EQ(result, "hello;world;test");
}

TEST_F(ArenaUtilsTest, JoinStringViewMultiCharDelimiter) {
    const std::string_view parts[] = {"hello", "world"};
    const auto result = join(std::span(parts), " | ", arena);

    EXPECT_EQ(result, "hello | world");
}

// ============================================================================
// join() Tests with string
// ============================================================================

TEST_F(ArenaUtilsTest, JoinStringSimple) {
    const std::string parts[] = {"hello", "world", "test"};
    const auto result = join(std::span(parts), ",", arena);

    EXPECT_EQ(result, "hello,world,test");
}

TEST_F(ArenaUtilsTest, JoinStringSinglePart) {
    const std::string parts[] = {"hello"};
    const auto result = join(std::span(parts), ",", arena);

    EXPECT_EQ(result, "hello");
}

TEST_F(ArenaUtilsTest, JoinStringEmpty) {
    std::vector<std::string> parts;
    const auto result = join(std::span(parts), ",", arena);

    EXPECT_EQ(result, "");
}

TEST_F(ArenaUtilsTest, JoinStringWithEmptyParts) {
    const std::string parts[] = {"hello", "", "world"};
    const auto result = join(std::span(parts), ",", arena);

    EXPECT_EQ(result, "hello,,world");
}

TEST_F(ArenaUtilsTest, JoinStringDifferentDelimiter) {
    const std::string parts[] = {"hello", "world", "test"};
    const auto result = join(std::span(parts), ";", arena);

    EXPECT_EQ(result, "hello;world;test");
}

// ============================================================================
// sprintf() Tests - Printf-style (variadic)
// ============================================================================

TEST_F(ArenaUtilsTest, SprintfSimple) {
    const auto result = sprintf(arena, "Hello, World!");

    EXPECT_EQ(result, "Hello, World!");
}

TEST_F(ArenaUtilsTest, SprintfWithInteger) {
    const auto result = sprintf(arena, "The answer is %d", 42);

    EXPECT_EQ(result, "The answer is 42");
}

TEST_F(ArenaUtilsTest, SprintfWithMultipleIntegers) {
    const auto result = sprintf(arena, "%d + %d = %d", 5, 3, 8);

    EXPECT_EQ(result, "5 + 3 = 8");
}

TEST_F(ArenaUtilsTest, SprintfWithString) {
    const auto result = sprintf(arena, "Hello, %s!", "World");

    EXPECT_EQ(result, "Hello, World!");
}

TEST_F(ArenaUtilsTest, SprintfWithFloat) {
    const auto result = sprintf(arena, "Pi is approximately %.2f", 3.14159);

    EXPECT_EQ(result, "Pi is approximately 3.14");
}

TEST_F(ArenaUtilsTest, SprintfWithHex) {
    const auto result = sprintf(arena, "Hex: 0x%x", 255);

    EXPECT_EQ(result, "Hex: 0xff");
}

TEST_F(ArenaUtilsTest, SprintfComplexFormat) {
    const auto result = sprintf(arena, "Name: %s, Age: %d, Score: %.1f", "Alice", 30, 95.5);

    EXPECT_EQ(result, "Name: Alice, Age: 30, Score: 95.5");
}

TEST_F(ArenaUtilsTest, SprintfEmpty) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-zero-length"
    const auto result = sprintf(arena, "");
#pragma GCC diagnostic pop

    EXPECT_EQ(result, "");
}

TEST_F(ArenaUtilsTest, SprintfLongString) {
    const auto result = sprintf(arena,
        "This is a longer string with multiple words and some numbers: %d, %d, %d",
        100, 200, 300
    );

    EXPECT_EQ(result, "This is a longer string with multiple words and some numbers: 100, 200, 300");
}

TEST_F(ArenaUtilsTest, SprintfWithPercent) {
    const auto result = sprintf(arena, "Progress: %d%%", 75);

    EXPECT_EQ(result, "Progress: 75%");
}

// ============================================================================
// Integration Tests
// ============================================================================

TEST_F(ArenaUtilsTest, SplitThenJoin) {
    const std::string_view input = "apple,banana,cherry";
    const ArenaAllocator<std::string_view> allocator(arena);

    auto parts = split(input, ',', allocator);

    // Convert string_view to string for join
    std::vector<std::string> strings;
    for (const auto& part : parts) {
        strings.emplace_back(part);
    }

    const auto result = join(std::span(strings), ",", arena);

    EXPECT_EQ(result, input);
}

TEST_F(ArenaUtilsTest, MultipleOperationsOnArena) {
    const ArenaAllocator<std::string_view> allocator(arena);

    // Split operation
    auto parts1 = split("a,b,c", ',', allocator);
    EXPECT_EQ(parts1.size(), 3);

    // split operation again
    auto parts2 = split("x;y;z", ';', allocator);
    EXPECT_EQ(parts2.size(), 3);

    // Arena should still have space for all allocations
    EXPECT_TRUE(true);  // If we got here without crash, arena management worked
}

TEST_F(ArenaUtilsTest, SplitLargeString) {
    std::string largeInput;
    for (int i = 0; i < 100; i++) {
        if (i > 0) largeInput += ",";
        largeInput += std::to_string(i);
    }

    const ArenaAllocator<std::string_view> allocator(arena);
    auto parts = split(largeInput, ',', allocator);

    EXPECT_EQ(parts.size(), 100);
    EXPECT_EQ(parts[0], "0");
    EXPECT_EQ(parts[50], "50");
    EXPECT_EQ(parts[99], "99");
}

TEST_F(ArenaUtilsTest, JoinManyParts) {
    std::vector<std::string> parts;
    for (int i = 0; i < 50; i++) {
        parts.push_back(std::to_string(i));
    }

    const auto result = join(std::span(parts), ",", arena);

    // Result should contain all numbers separated by commas
    EXPECT_NE(result.find("0"), std::string::npos);
    EXPECT_NE(result.find("49"), std::string::npos);
    EXPECT_GT(result.length(), 100);  // Should be fairly long
}