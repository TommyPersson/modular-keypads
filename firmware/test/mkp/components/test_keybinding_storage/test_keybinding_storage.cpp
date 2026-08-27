#include <gtest/gtest.h>
#include <tfw/hal/fs/MockFileSystem.h>
#include "mkp/components/keybindings/KeyBindingStorage.h"
#include <string>
#include <vector>
#include <memory>

#include "mkp/components/keybindings/KeyBindingStorage.cpp"

using namespace tfw::hal::fs;
using namespace mkp::components::keybindings;

class KeyBindingStorageTest : public ::testing::Test {
protected:
    MockFileSystem mockFs;
    KeyBindingStorage keyBindingStorage{mockFs};
    const char* testFilePath = "/data/key-bindings.txt";
    const char* tempFilePath = "/data/key-bindings.txt.tmp";

    void SetUp() override {
        mockFs.begin();
        keyBindingStorage.setup();
    }

    void TearDown() override {
        mockFs.clear();
    }

    // Helper functions for creating test key bindings
    KeyBinding createPushButtonBinding(uint64_t deviceId, int number, uint16_t macroId) {
        auto trigger = std::make_shared<PushButtonTrigger>(deviceId, number);
        return KeyBinding{trigger, macroId};
    }

    KeyBinding createRotaryEncoderBinding(uint64_t deviceId, int number,
                                          tfw::hal::encoders::RotaryEncoderDirection direction, uint16_t macroId) {
        auto trigger = std::make_shared<RotaryEncoderTrigger>(deviceId, number, direction);
        return KeyBinding{trigger, macroId};
    }
};

// ============================================================================
// Basic Write Tests
// ============================================================================

TEST_F(KeyBindingStorageTest, WritePushButtonBinding) {
    auto binding = createPushButtonBinding(0x0001, 5, 100);

    EXPECT_EQ(keyBindingStorage.write(binding), 0);
    EXPECT_TRUE(mockFs.exists(testFilePath));
}

TEST_F(KeyBindingStorageTest, WriteRotaryEncoderBinding) {
    auto binding = createRotaryEncoderBinding(0x0002, 3, tfw::hal::encoders::RotaryEncoderDirection::Clockwise, 200);

    EXPECT_EQ(keyBindingStorage.write(binding), 0);
    EXPECT_TRUE(mockFs.exists(testFilePath));
}

TEST_F(KeyBindingStorageTest, WriteMultipleBindings) {
    auto binding1 = createPushButtonBinding(0x0001, 1, 10);
    auto binding2 = createRotaryEncoderBinding(0x0002, 1, tfw::hal::encoders::RotaryEncoderDirection::Clockwise, 20);

    EXPECT_EQ(keyBindingStorage.write(binding1), 0);
    EXPECT_EQ(keyBindingStorage.write(binding2), 0);

    EXPECT_EQ(keyBindingStorage.count(), 2);
}

TEST_F(KeyBindingStorageTest, OverwriteExistingBinding) {
    auto binding1 = createPushButtonBinding(0x0001, 5, 100);
    auto binding2 = createPushButtonBinding(0x0001, 5, 200);  // Same trigger, different macro

    EXPECT_EQ(keyBindingStorage.write(binding1), 0);

    EXPECT_EQ(keyBindingStorage.count(), 1);

    EXPECT_EQ(keyBindingStorage.write(binding2), 0);

    EXPECT_EQ(keyBindingStorage.count(), 1);  // Still only 1, not 2
}

// ============================================================================
// Remove Tests
// ============================================================================

TEST_F(KeyBindingStorageTest, RemoveExistingBinding) {
    auto binding1 = createPushButtonBinding(0x0001, 1, 10);
    auto binding2 = createPushButtonBinding(0x0001, 2, 20);

    keyBindingStorage.write(binding1);
    keyBindingStorage.write(binding2);

    PushButtonTrigger trigger1(0x0001, 1);
    EXPECT_EQ(keyBindingStorage.remove(trigger1), 0);

    EXPECT_EQ(keyBindingStorage.count(), 1);
}

TEST_F(KeyBindingStorageTest, RemoveNonExistentBinding) {
    auto binding = createPushButtonBinding(0x0001, 1, 10);
    keyBindingStorage.write(binding);

    PushButtonTrigger nonExistentTrigger(0x0002, 1);
    EXPECT_EQ(keyBindingStorage.remove(nonExistentTrigger), 0);

    EXPECT_EQ(keyBindingStorage.count(), 1);  // Original binding still there
}

TEST_F(KeyBindingStorageTest, RemoveAllBindingsForMacro) {
    auto binding1 = createPushButtonBinding(0x0001, 1, 10);
    auto binding2 = createPushButtonBinding(0x0001, 2, 10);
    auto binding3 = createPushButtonBinding(0x0001, 3, 20);

    keyBindingStorage.write(binding1);
    keyBindingStorage.write(binding2);
    keyBindingStorage.write(binding3);

    EXPECT_EQ(keyBindingStorage.removeAll(10), 0);

    EXPECT_EQ(keyBindingStorage.count(), 1);  // Only binding3 remains
}

TEST_F(KeyBindingStorageTest, RemoveAllBindingsForNonExistentMacro) {
    auto binding = createPushButtonBinding(0x0001, 1, 10);
    keyBindingStorage.write(binding);

    EXPECT_EQ(keyBindingStorage.removeAll(999), 0);

    EXPECT_EQ(keyBindingStorage.count(), 1);  // Original binding still there
}

// ============================================================================
// ForEach Tests
// ============================================================================

TEST_F(KeyBindingStorageTest, ForEachEmptyStorage) {
    EXPECT_EQ(keyBindingStorage.count(), 0);
}

TEST_F(KeyBindingStorageTest, ForEachSingleBinding) {
    auto binding = createPushButtonBinding(0x0001, 5, 100);
    keyBindingStorage.write(binding);

    EXPECT_EQ(keyBindingStorage.count(), 1);

    keyBindingStorage.forEach([&](const KeyBinding& b) {
        EXPECT_EQ(b.macroId, 100);
    });
}

TEST_F(KeyBindingStorageTest, ForEachMultipleBindings) {
    auto binding1 = createPushButtonBinding(0x0001, 1, 10);
    auto binding2 = createRotaryEncoderBinding(0x0002, 1, tfw::hal::encoders::RotaryEncoderDirection::Clockwise, 20);
    auto binding3 = createPushButtonBinding(0x0003, 1, 30);

    keyBindingStorage.write(binding1);
    keyBindingStorage.write(binding2);
    keyBindingStorage.write(binding3);

    EXPECT_EQ(keyBindingStorage.count(), 3);

    std::vector<uint16_t> macroIds;
    keyBindingStorage.forEach([&](const KeyBinding& b) {
        macroIds.push_back(b.macroId);
    });

    EXPECT_EQ(macroIds[0], 10);
    EXPECT_EQ(macroIds[1], 20);
    EXPECT_EQ(macroIds[2], 30);
}

// ============================================================================
// Observable Tests
// ============================================================================

class KeyBindingSetObserver : public tfw::utils::observables::Observer<KeyBindingSet> {
public:
    int call_count = 0;

    void observe(const KeyBindingSet&) override {
        call_count++;
    }
};

class KeyBindingClearedObserver : public tfw::utils::observables::Observer<KeyBindingCleared> {
public:
    int call_count = 0;

    void observe(const KeyBindingCleared&) override {
        call_count++;
    }
};

TEST_F(KeyBindingStorageTest, OnKeyBindingSetNotification) {
    auto binding = createPushButtonBinding(0x0001, 1, 10);

    KeyBindingSetObserver observer;
    keyBindingStorage.onKeyBindingSet().addObserver(&observer);

    keyBindingStorage.write(binding);
    EXPECT_EQ(observer.call_count, 1);
}

TEST_F(KeyBindingStorageTest, OnKeyBindingClearedNotification) {
    auto binding = createPushButtonBinding(0x0001, 1, 10);
    keyBindingStorage.write(binding);

    KeyBindingClearedObserver observer;
    keyBindingStorage.onKeyBindingCleared().addObserver(&observer);

    PushButtonTrigger trigger(0x0001, 1);
    keyBindingStorage.remove(trigger);
    EXPECT_EQ(observer.call_count, 1);
}

TEST_F(KeyBindingStorageTest, OnKeyBindingClearedNotificationOnRemoveAll) {
    auto binding1 = createPushButtonBinding(0x0001, 1, 10);
    auto binding2 = createPushButtonBinding(0x0001, 2, 10);
    keyBindingStorage.write(binding1);
    keyBindingStorage.write(binding2);

    KeyBindingClearedObserver observer;
    keyBindingStorage.onKeyBindingCleared().addObserver(&observer);

    keyBindingStorage.removeAll(10);
    EXPECT_EQ(observer.call_count, 1);
}

TEST_F(KeyBindingStorageTest, MultipleObserversNotified) {
    auto binding = createPushButtonBinding(0x0001, 1, 10);

    KeyBindingSetObserver observer1, observer2;
    keyBindingStorage.onKeyBindingSet().addObserver(&observer1);
    keyBindingStorage.onKeyBindingSet().addObserver(&observer2);

    keyBindingStorage.write(binding);
    EXPECT_EQ(observer1.call_count, 1);
    EXPECT_EQ(observer2.call_count, 1);
}

// ============================================================================
// Mixed Trigger Types Tests
// ============================================================================

TEST_F(KeyBindingStorageTest, MixedTriggerTypes) {
    auto pushButton = createPushButtonBinding(0x0001, 1, 10);
    auto rotaryClockwise = createRotaryEncoderBinding(0x0002, 1, tfw::hal::encoders::RotaryEncoderDirection::Clockwise, 20);
    auto rotaryCounterClockwise = createRotaryEncoderBinding(0x0003, 1, tfw::hal::encoders::RotaryEncoderDirection::CounterClockwise, 30);

    keyBindingStorage.write(pushButton);
    keyBindingStorage.write(rotaryClockwise);
    keyBindingStorage.write(rotaryCounterClockwise);

    int count = 0;
    keyBindingStorage.forEach([&](const KeyBinding&) {
        count++;
    });

    EXPECT_EQ(count, 3);
}

TEST_F(KeyBindingStorageTest, MultipleDevices) {
    auto device1_button1 = createPushButtonBinding(0x0001, 1, 10);
    auto device1_button2 = createPushButtonBinding(0x0001, 2, 20);
    auto device2_button1 = createPushButtonBinding(0x0002, 1, 30);
    auto device2_encoder = createRotaryEncoderBinding(0x0002, 1, tfw::hal::encoders::RotaryEncoderDirection::Clockwise, 40);

    keyBindingStorage.write(device1_button1);
    keyBindingStorage.write(device1_button2);
    keyBindingStorage.write(device2_button1);
    keyBindingStorage.write(device2_encoder);

    int count = 0;
    keyBindingStorage.forEach([&](const KeyBinding&) {
        count++;
    });

    EXPECT_EQ(count, 4);
}

// ============================================================================
// Edge Cases
// ============================================================================

TEST_F(KeyBindingStorageTest, LargeMacroId) {
    auto binding = createPushButtonBinding(0x0001, 1, 65535);  // Max uint16_t

    EXPECT_EQ(keyBindingStorage.write(binding), 0);

    int count = 0;
    uint16_t macroId = 0;
    keyBindingStorage.forEach([&](const KeyBinding& b) {
        count++;
        macroId = b.macroId;
    });

    EXPECT_EQ(count, 1);
    EXPECT_EQ(macroId, 65535);
}

TEST_F(KeyBindingStorageTest, LargeDeviceId) {
    auto binding = createPushButtonBinding(0xFFFFFFFFFFFFFFFF, 1, 10);  // Max uint64_t

    EXPECT_EQ(keyBindingStorage.write(binding), 0);

    EXPECT_EQ(keyBindingStorage.count(), 1);
}

TEST_F(KeyBindingStorageTest, SequentialOperations) {
    auto binding1 = createPushButtonBinding(0x0001, 1, 10);
    auto binding2 = createPushButtonBinding(0x0001, 2, 20);
    auto binding3 = createPushButtonBinding(0x0001, 3, 30);

    // Write
    keyBindingStorage.write(binding1);
    keyBindingStorage.write(binding2);
    keyBindingStorage.write(binding3);

    // Verify count
    int countAfterWrite = 0;
    keyBindingStorage.forEach([&](const KeyBinding&) {
        countAfterWrite++;
    });
    EXPECT_EQ(countAfterWrite, 3);

    // Remove one
    PushButtonTrigger trigger2(0x0001, 2);
    keyBindingStorage.remove(trigger2);

    int countAfterRemove = 0;
    keyBindingStorage.forEach([&](const KeyBinding&) {
        countAfterRemove++;
    });
    EXPECT_EQ(countAfterRemove, 2);

    // Remove all for macro 10
    keyBindingStorage.removeAll(10);

    int countAfterRemoveAll = 0;
    keyBindingStorage.forEach([&](const KeyBinding&) {
        countAfterRemoveAll++;
    });
    EXPECT_EQ(countAfterRemoveAll, 1);
}
