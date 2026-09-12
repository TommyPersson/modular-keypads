#include <gtest/gtest.h>
#include <span>
#include <vector>
#include <list>
#include "tfw/utils/allocations.h"

using namespace tfw::utils::allocations;

class ArenaTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Set up common test data
    }
};

// ============================================================================
// Basic Arena Tests
// ============================================================================

TEST_F(ArenaTest, CreateArena) {
    Arena arena(1024);
    EXPECT_NE(arena.getBuffer(), nullptr);
}

TEST_F(ArenaTest, AllocateFromArena) {
    Arena arena(1024);
    auto ptr = arena.allocate(100);

    EXPECT_NE(ptr, nullptr);
    EXPECT_EQ(ptr, arena.getBuffer());  // First allocation should be at buffer start
}

TEST_F(ArenaTest, AllocateMultiple) {
    Arena arena(1024);
    auto ptr1 = arena.allocate(100);
    auto ptr2 = arena.allocate(100);

    EXPECT_NE(ptr1, nullptr);
    EXPECT_NE(ptr2, nullptr);
    EXPECT_EQ(ptr2 - ptr1, 100);  // Second allocation should be offset by 100
}

TEST_F(ArenaTest, AllocateTooLarge) {
    Arena arena(100);
    auto ptr = arena.allocate(200);  // Request more than arena size

    EXPECT_EQ(ptr, nullptr);
}

TEST_F(ArenaTest, AllocateExactSize) {
    Arena arena(100);
    auto ptr = arena.allocate(100);

    EXPECT_NE(ptr, nullptr);
}

TEST_F(ArenaTest, AllocateOneByteOver) {
    Arena arena(100);
    auto ptr = arena.allocate(101);

    EXPECT_EQ(ptr, nullptr);
}

TEST_F(ArenaTest, ResetArena) {
    Arena arena(1024);

    auto ptr1 = arena.allocate(100);
    auto offset1 = ptr1;

    arena.reset();

    auto ptr2 = arena.allocate(50);
    auto offset2 = ptr2;

    EXPECT_EQ(offset1, offset2);  // After reset, allocations start at buffer start again
}

TEST_F(ArenaTest, BufferInitializedToZero) {
    Arena arena(100);
    auto buffer = arena.getBuffer();

    for (size_t i = 0; i < 100; ++i) {
        EXPECT_EQ(buffer[i], 0);
    }
}

TEST_F(ArenaTest, WriteToAllocatedMemory) {
    Arena arena(1024);
    auto ptr = arena.allocate(10);

    memcpy(ptr, "hello", 5);

    EXPECT_EQ(memcmp(ptr, "hello", 5), 0);
}

// ============================================================================
// ArenaAllocator Tests
// ============================================================================

TEST_F(ArenaTest, ArenaAllocatorAllocate) {
    Arena arena(1024);
    ArenaAllocator<int> allocator(arena);

    auto ptr = allocator.allocate(10);  // Allocate for 10 ints

    EXPECT_NE(ptr, nullptr);
}

TEST_F(ArenaTest, ArenaAllocatorAllocateMultipleTypes) {
    Arena arena(1024);

    ArenaAllocator<int> intAllocator(arena);
    ArenaAllocator<double> doubleAllocator(arena);

    auto intPtr = intAllocator.allocate(10);
    auto doublePtr = doubleAllocator.allocate(10);

    EXPECT_NE(intPtr, nullptr);
    EXPECT_NE(doublePtr, nullptr);
}

TEST_F(ArenaTest, ArenaAllocatorDeallocateIsNoop) {
    Arena arena(1024);
    ArenaAllocator<int> allocator(arena);

    auto ptr = allocator.allocate(10);
    allocator.deallocate(ptr, 10);  // Should not crash

    // Allocation offset should not change
    auto ptr2 = allocator.allocate(10);
    // ptr2 - ptr with int pointers = units of sizeof(int)
    // Allocating 10 ints = 40 bytes, so ptr2 is 10 ints after ptr
    EXPECT_EQ(ptr2 - ptr, 10);
}

TEST_F(ArenaTest, ArenaAllocatorWithVector) {
    Arena arena(4096);
    ArenaAllocator<int> allocator(arena);

    arena::vector<int> vec(allocator);
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);

    EXPECT_EQ(vec.size(), 3);
    EXPECT_EQ(vec[0], 1);
    EXPECT_EQ(vec[1], 2);
    EXPECT_EQ(vec[2], 3);
}

TEST_F(ArenaTest, ArenaAllocatorWithList) {
    Arena arena(4096);
    ArenaAllocator<int> allocator(arena);

    arena::list<int> list(allocator);
    list.push_back(1);
    list.push_back(2);
    list.push_back(3);

    EXPECT_EQ(list.size(), 3);
    auto it = list.begin();
    EXPECT_EQ(*it, 1);
    ++it;
    EXPECT_EQ(*it, 2);
}

TEST_F(ArenaTest, ArenaAllocatorWithString) {
    Arena arena(4096);
    ArenaAllocator<char> allocator(arena);

    arena::string str(allocator);
    str = "hello";

    EXPECT_EQ(str, "hello");
    EXPECT_EQ(str.length(), 5);
}

// ============================================================================
// ArenaAllocator Equality Tests
// ============================================================================

TEST_F(ArenaTest, AllocatorsFromSameArenaCompareEqual) {
    Arena arena(1024);
    ArenaAllocator<int> allocator1(arena);
    ArenaAllocator<int> allocator2(arena);

    EXPECT_TRUE(allocator1 == allocator2);
}

TEST_F(ArenaTest, AllocatorsFromDifferentArenasCompareNotEqual) {
    Arena arena1(1024);
    Arena arena2(1024);

    ArenaAllocator<int> allocator1(arena1);
    ArenaAllocator<int> allocator2(arena2);

    // BUG WARNING: This test expects allocators from DIFFERENT arenas to compare unequal,
    // but the current implementation always returns true for operator==.
    // This violates the C++ allocator contract and could cause issues with
    // container equality comparisons and moves.
    EXPECT_FALSE(allocator1 == allocator2);
}

TEST_F(ArenaTest, DifferentAllocatorTypesCompareEqual) {
    Arena arena(1024);
    ArenaAllocator<int> intAllocator(arena);
    ArenaAllocator<double> doubleAllocator(arena);

    // This will compile because of the template operator== overload
    // But the semantics are questionable - allocators from same arena should compare equal
    EXPECT_TRUE(intAllocator == doubleAllocator);
}

// ============================================================================
// ArenaUtils Tests - DEFERRED
// ============================================================================
//
// ArenaUtils functions (sprintf, split, join) are not yet tested.
// See TEST_ARENA_UTILS_TEMPLATE.md for comprehensive test suite.
//
// ArenaUtils.cpp is in lib/tfw/src/tfw/utils/allocations/ but requires
// special build configuration to compile. Extra_script approach attempted
// but needs further investigation with PlatformIO's build system.
//
// Known issue for future testing:
// BUG (ArenaUtils.cpp:90): sprintf() returns std::string_view with size
// that includes null terminator - should exclude it for proper semantics.


// ============================================================================
// Edge Cases
// ============================================================================

TEST_F(ArenaTest, MultipleArenasIndependent) {
    Arena arena1(1024);
    Arena arena2(1024);

    auto ptr1 = arena1.allocate(100);
    auto ptr2 = arena2.allocate(100);

    // Each arena should have its own offset
    EXPECT_NE(ptr1, ptr2);

    // Resetting one shouldn't affect the other
    arena1.reset();
    auto ptr1_after = arena1.allocate(50);
    auto ptr2_check = arena2.allocate(50);

    // arena1's allocation should be at the start again
    EXPECT_EQ(ptr1_after, arena1.getBuffer());

    // arena2's allocation should be after its previous one
    EXPECT_EQ(ptr2_check - ptr2, 100);
}
