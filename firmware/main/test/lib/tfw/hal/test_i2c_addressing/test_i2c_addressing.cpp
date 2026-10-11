#include <gtest/gtest.h>
#include <set>
#include "tfw/hal/i2c/AddressAllocator.h"
#include "tfw/hal/i2c/Endpoint.h"

using namespace tfw::hal::i2c;

namespace {
    // Reads from several devices at the same address are the bitwise AND of their responses
    Identity readFromAll(const std::initializer_list<uint64_t> ids) {
        Identity result{.id = ~0ull, .idComplement = ~0ull};
        for (const auto id : ids) {
            const auto identity = Identity::of(id);
            result.id &= identity.id;
            result.idComplement &= identity.idComplement;
        }
        return result;
    }
}

class AddressAllocatorTest : public ::testing::Test {
protected:
    std::optional<uint8_t> allocate(const uint64_t deviceId) {
        return allocator.allocate(deviceId, [this](const uint8_t address) { return !occupied.contains(address); });
    }

    AddressAllocator allocator;

    // Addresses that devices answer at
    std::set<uint8_t> occupied;
};

TEST(IdentityTest, IdentityOfSingleDeviceIsValid) {
    EXPECT_TRUE(readFromAll({0x0123456789abcdefull}).isValid());
}

TEST(IdentityTest, IdentityOfCollidingDevicesIsInvalid) {
    EXPECT_FALSE(readFromAll({0x0123456789abcdefull, 0x0123456789abcdeeull}).isValid());
    EXPECT_FALSE(readFromAll({1, 2, 3}).isValid());
}

TEST(IdentityTest, IdentityOfNoResponseIsInvalid) {
    EXPECT_FALSE((Identity{}).isValid());
    EXPECT_FALSE((Identity{.id = ~0ull, .idComplement = ~0ull}).isValid());
}

TEST_F(AddressAllocatorTest, AllocatesLowestPermanentAddress) {
    EXPECT_EQ(allocate(1), addresses::PERMANENT_FIRST);
}

TEST_F(AddressAllocatorTest, AllocatesDifferentAddressesToDifferentDevices) {
    EXPECT_EQ(allocate(1), addresses::PERMANENT_FIRST);
    EXPECT_EQ(allocate(2), addresses::PERMANENT_FIRST + 1);
}

TEST_F(AddressAllocatorTest, SkipsOccupiedAddresses) {
    occupied = {addresses::PERMANENT_FIRST, addresses::PERMANENT_FIRST + 1};

    EXPECT_EQ(allocate(1), addresses::PERMANENT_FIRST + 2);
}

TEST_F(AddressAllocatorTest, AllocatesPreviousAddressToRestartedDevice) {
    allocate(1);
    allocate(2);
    allocate(3);

    EXPECT_EQ(allocate(2), addresses::PERMANENT_FIRST + 1);
}

TEST_F(AddressAllocatorTest, AllocatesNewAddressWhenPreviousIsOccupied) {
    allocate(1);
    occupied = {addresses::PERMANENT_FIRST};

    EXPECT_EQ(allocate(1), addresses::PERMANENT_FIRST + 1);
    EXPECT_EQ(allocator.addressOf(1), addresses::PERMANENT_FIRST + 1);
}

TEST_F(AddressAllocatorTest, KeepsAddressesOfGoneDevicesForThemWhileOthersAreFree) {
    allocate(1);

    // Device 1 is gone, its address answers no more
    EXPECT_EQ(allocate(2), addresses::PERMANENT_FIRST + 1);
}

TEST_F(AddressAllocatorTest, ReusesAddressesOfGoneDevicesWhenNoOthersAreFree) {
    for (uint64_t id = 1; id <= addresses::PERMANENT_LAST - addresses::PERMANENT_FIRST + 1; id++) {
        allocate(id);
    }

    // All devices are gone, except the first
    occupied = {addresses::PERMANENT_FIRST};

    EXPECT_EQ(allocate(1000), addresses::PERMANENT_FIRST + 1);
    EXPECT_EQ(allocator.addressOf(1000), addresses::PERMANENT_FIRST + 1);
    EXPECT_EQ(allocator.addressOf(2), std::nullopt);
}

TEST_F(AddressAllocatorTest, ReturnsNothingWhenAllAddressesAreOccupied) {
    for (int address = addresses::PERMANENT_FIRST; address <= addresses::PERMANENT_LAST; address++) {
        occupied.insert(address);
    }

    EXPECT_EQ(allocate(1), std::nullopt);
    EXPECT_EQ(allocator.addressOf(1), std::nullopt);
}

TEST_F(AddressAllocatorTest, RememberedAddressIsAllocatedAgain) {
    allocator.remember(1, 0x20);

    EXPECT_EQ(allocate(1), 0x20);
}

TEST_F(AddressAllocatorTest, RememberingAddressForgetsOtherDeviceAtIt) {
    allocate(1);
    allocator.remember(2, addresses::PERMANENT_FIRST);

    EXPECT_EQ(allocator.addressOf(1), std::nullopt);
    EXPECT_EQ(allocator.addressOf(2), addresses::PERMANENT_FIRST);
}

TEST_F(AddressAllocatorTest, RememberedAddressesAreNotAllocatedToOthers) {
    allocator.remember(1, addresses::PERMANENT_FIRST);

    EXPECT_EQ(allocate(2), addresses::PERMANENT_FIRST + 1);
}
