#include <gtest/gtest.h>
#include "tfw/hal/settings/CachedDeviceSettings.h"
#include "tfw/hal/settings/MockDeviceSettings.h"

using namespace tfw::hal::settings;

class CachedDeviceSettingsTest : public ::testing::Test {
protected:
    MockDeviceSettings backing;
    CachedDeviceSettings settings{backing};
};

TEST_F(CachedDeviceSettingsTest, GetReturnsDefaultForMissingKey) {
    EXPECT_EQ(settings.getUInt("missing", 42), 42u);
}

TEST_F(CachedDeviceSettingsTest, GetReturnsStoredValue) {
    backing.putUInt("key", 7);

    EXPECT_EQ(settings.getUInt("key", 42), 7u);
}

TEST_F(CachedDeviceSettingsTest, RepeatedGetOnlyReadsBackingOnce) {
    backing.putUInt("key", 7);

    settings.getUInt("key", 0);
    const auto numReads = backing.numReads;

    EXPECT_EQ(settings.getUInt("key", 0), 7u);
    EXPECT_EQ(settings.getUInt("key", 0), 7u);
    EXPECT_EQ(backing.numReads, numReads);
}

TEST_F(CachedDeviceSettingsTest, MissingKeyIsCached) {
    settings.getUInt("missing", 1);
    const auto numReads = backing.numReads;

    EXPECT_EQ(settings.getUInt("missing", 2), 2u);
    EXPECT_FALSE(settings.isKey("missing"));
    EXPECT_EQ(backing.numReads, numReads);
}

TEST_F(CachedDeviceSettingsTest, PutWritesThroughAndUpdatesCache) {
    settings.getUInt("key", 0);

    EXPECT_TRUE(settings.putUInt("key", 5));
    const auto numReads = backing.numReads;

    EXPECT_EQ(settings.getUInt("key", 0), 5u);
    EXPECT_EQ(backing.getUInt("key", 0), 5u);
    EXPECT_EQ(backing.numReads, numReads + 1); // Only the direct read of the backing
}

TEST_F(CachedDeviceSettingsTest, FailedPutClearsCachedValue) {
    backing.putUInt("key", 5);
    settings.getUInt("key", 0);

    backing.failWrites = true;
    EXPECT_FALSE(settings.putUInt("key", 6));

    EXPECT_EQ(settings.getUInt("key", 0), 5u);
}

TEST_F(CachedDeviceSettingsTest, RemoveMakesKeyMissing) {
    settings.putUInt("key", 5);

    EXPECT_TRUE(settings.remove("key"));

    EXPECT_FALSE(settings.isKey("key"));
    EXPECT_EQ(settings.getUInt("key", 9), 9u);
    EXPECT_FALSE(backing.isKey("key"));
}

TEST_F(CachedDeviceSettingsTest, IsKeyReturnsTrueForCachedValue) {
    settings.putUInt("key", 5);
    const auto numReads = backing.numReads;

    EXPECT_TRUE(settings.isKey("key"));
    EXPECT_EQ(backing.numReads, numReads);
}

TEST_F(CachedDeviceSettingsTest, SupportsAllValueTypes) {
    settings.putInt("int", -3);
    settings.putBool("bool", true);

    EXPECT_EQ(settings.getInt("int", 0), -3);
    EXPECT_TRUE(settings.getBool("bool", false));
    EXPECT_FALSE(settings.getBool("missing", false));
}

TEST_F(CachedDeviceSettingsTest, ValueCachedAsOtherTypeIsReadFromBacking) {
    settings.putUInt("key", 5);
    const auto numReads = backing.numReads;

    EXPECT_EQ(settings.getInt("key", -1), -1); // The mock doesn't convert between types
    EXPECT_GT(backing.numReads, numReads);
}

TEST_F(CachedDeviceSettingsTest, BeginClearsCache) {
    backing.putUInt("key", 5);
    settings.getUInt("key", 0);

    backing.putUInt("key", 6);
    settings.begin();

    EXPECT_EQ(settings.getUInt("key", 0), 6u);
}
