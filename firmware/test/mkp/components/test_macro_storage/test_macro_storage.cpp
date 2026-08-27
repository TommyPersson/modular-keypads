#include <gtest/gtest.h>
#include <tfw/hal/fs/MockFileSystem.h>
#include "mkp/components/macros/MacroStorage.h"
#include <string>
#include <vector>
#include <memory>

#include "mkp/components/macros/MacroStorage.cpp"
#include "mkp/components/macros/MacroDataSerializers.cpp"

using namespace tfw::hal::fs;
using namespace mkp::components::macros;

class MacroStorageTest : public ::testing::Test {
protected:
    MockFileSystem mockFs;
    MacroStorage macroStorage{mockFs};
    const char* testFilePath = "/data/macro-definitions.txt";
    const char* tempFilePath = "/data/macro-definitions.txt.tmp";

    void SetUp() override {
        mockFs.begin();
        macroStorage.setup();
    }

    void TearDown() override {
        mockFs.clear();
    }

    // Helper functions for creating test macros
    Macro createShortcutMacro(
        uint16_t id,
        const std::string_view& name,
        uint8_t modifiers = 0x01,
        uint8_t hidKeyCode = 0x04) {
        auto data = std::make_shared<ShortcutMacroData>(id, modifiers, hidKeyCode);
        return Macro{name, data};
    }

    Macro createConsumerControlMacro(
        uint16_t id,
        const std::string_view& name,
        uint16_t usageId = 0x00E9) {
        auto data = std::make_shared<ConsumerControlMacroData>(id, usageId);
        return Macro{name, data};
    }

    Macro createSystemControlMacro(
        uint16_t id,
        const std::string_view& name,
        uint8_t code = 0x01) {
        auto data = std::make_shared<SystemControlMacroData>(id, code);
        return Macro{name, data};
    }

    Macro createTextMacro(
        uint16_t id,
        const std::string_view& name,
        const std::string_view& text) {
        auto data = std::make_shared<TextMacroData>(id, text);
        return Macro{name, data};
    }
};

// ============================================================================
// Basic Write Tests
// ============================================================================

TEST_F(MacroStorageTest, WriteNewMacroAssignsId) {
    auto macro = createShortcutMacro(0, "Test Macro");

    EXPECT_EQ(macroStorage.write(macro), 0);
    EXPECT_EQ(macro.data->id, 1);  // Should assign ID 1
    EXPECT_TRUE(mockFs.exists(testFilePath));
}

TEST_F(MacroStorageTest, WriteNewMacroWithPositiveIdKeepsId) {
    auto macro = createShortcutMacro(5, "Test Macro");

    EXPECT_EQ(macroStorage.write(macro), 0);
    EXPECT_EQ(macro.data->id, 5);  // Should keep provided ID
}

TEST_F(MacroStorageTest, WriteMultipleMacros) {
    auto macro1 = createShortcutMacro(0, "Macro 1");
    auto macro2 = createShortcutMacro(0, "Macro 2");

    EXPECT_EQ(macroStorage.write(macro1), 0);
    EXPECT_EQ(macro1.data->id, 1);

    EXPECT_EQ(macroStorage.write(macro2), 0);
    EXPECT_EQ(macro2.data->id, 2);

    EXPECT_EQ(macroStorage.getNumStored(), 2);
}

TEST_F(MacroStorageTest, OverwriteExistingMacro) {
    auto macro1 = createShortcutMacro(1, "Original Macro");
    auto macro2 = createShortcutMacro(1, "Updated Macro");

    macroStorage.write(macro1);
    EXPECT_EQ(macroStorage.getNumStored(), 1);

    macroStorage.write(macro2);
    EXPECT_EQ(macroStorage.getNumStored(), 1);  // Still only 1 macro

    // Verify the name was updated
    std::string retrievedName;
    macroStorage.forEach([&](const Macro& m) {
        if (m.data->id == 1) {
            retrievedName = std::string(m.name);
        }
    });
    EXPECT_EQ(retrievedName, "Updated Macro");
}

TEST_F(MacroStorageTest, WriteShortcutMacro) {
    auto macro = createShortcutMacro(1, "Ctrl+A", 0x01, 0x04);

    EXPECT_EQ(macroStorage.write(macro), 0);

    std::string content = mockFs.getFileContent(testFilePath);
    EXPECT_NE(content.find("Ctrl+A"), std::string::npos);
    EXPECT_NE(content.find("0x01"), std::string::npos);  // modifiers
}

TEST_F(MacroStorageTest, WriteConsumerControlMacro) {
    auto macro = createConsumerControlMacro(1, "Volume Up", 0x00E9);

    EXPECT_EQ(macroStorage.write(macro), 0);

    std::string content = mockFs.getFileContent(testFilePath);
    EXPECT_NE(content.find("Volume Up"), std::string::npos);
    EXPECT_NE(content.find("0x00e9"), std::string::npos);
}

TEST_F(MacroStorageTest, WriteSystemControlMacro) {
    auto macro = createSystemControlMacro(1, "Sleep", 0x01);

    EXPECT_EQ(macroStorage.write(macro), 0);

    std::string content = mockFs.getFileContent(testFilePath);
    EXPECT_NE(content.find("Sleep"), std::string::npos);
    EXPECT_NE(content.find("0x01"), std::string::npos);
}

TEST_F(MacroStorageTest, WriteTextMacro) {
    auto macro = createTextMacro(1, "Password", "MySecretPassword123");

    EXPECT_EQ(macroStorage.write(macro), 0);

    std::string content = mockFs.getFileContent(testFilePath);
    EXPECT_NE(content.find("Password"), std::string::npos);
    EXPECT_NE(content.find("MySecretPassword123"), std::string::npos);
}

// ============================================================================
// Remove Tests
// ============================================================================

TEST_F(MacroStorageTest, RemoveExistingMacro) {
    auto macro1 = createShortcutMacro(1, "Macro 1");
    auto macro2 = createShortcutMacro(2, "Macro 2");

    macroStorage.write(macro1);
    macroStorage.write(macro2);
    EXPECT_EQ(macroStorage.getNumStored(), 2);

    EXPECT_EQ(macroStorage.remove(1), 0);
    EXPECT_EQ(macroStorage.getNumStored(), 1);
}

TEST_F(MacroStorageTest, RemoveNonExistentMacro) {
    auto macro = createShortcutMacro(1, "Macro");
    macroStorage.write(macro);

    // Removing non-existent macro should still succeed
    EXPECT_EQ(macroStorage.remove(999), 0);
    EXPECT_EQ(macroStorage.getNumStored(), 1);
}

TEST_F(MacroStorageTest, RemoveAllMacros) {
    auto macro1 = createShortcutMacro(1, "Macro 1");
    auto macro2 = createShortcutMacro(2, "Macro 2");

    macroStorage.write(macro1);
    macroStorage.write(macro2);

    macroStorage.remove(1);
    macroStorage.remove(2);

    EXPECT_EQ(macroStorage.getNumStored(), 0);
}

TEST_F(MacroStorageTest, RemoveUpdatesNumStoredCache) {
    auto macro1 = createShortcutMacro(1, "Macro 1");
    auto macro2 = createShortcutMacro(2, "Macro 2");

    macroStorage.write(macro1);
    macroStorage.write(macro2);

    // Cache the count
    EXPECT_EQ(macroStorage.getNumStored(), 2);

    // Remove and verify cache is invalidated
    macroStorage.remove(1);
    EXPECT_EQ(macroStorage.getNumStored(), 1);
}

// ============================================================================
// ForEach Tests
// ============================================================================

TEST_F(MacroStorageTest, ForEachEmptyStorage) {
    EXPECT_EQ(macroStorage.count(), 0);
}

TEST_F(MacroStorageTest, ForEachSingleMacro) {
    auto macro = createShortcutMacro(1, "Test");
    macroStorage.write(macro);

    EXPECT_EQ(macroStorage.count(), 1);

    macroStorage.forEach([&](const Macro& m) {
        EXPECT_EQ(m.data->id, 1);
        EXPECT_EQ(m.name, "Test");
    });
}

TEST_F(MacroStorageTest, ForEachMultipleMacros) {
    auto macro1 = createShortcutMacro(1, "Macro 1");
    auto macro2 = createConsumerControlMacro(2, "Macro 2");
    auto macro3 = createTextMacro(3, "Macro 3", "text");

    macroStorage.write(macro1);
    macroStorage.write(macro2);
    macroStorage.write(macro3);

    std::vector<uint16_t> ids;
    macroStorage.forEach([&](const Macro& m) {
        ids.push_back(m.data->id);
    });

    EXPECT_EQ(ids.size(), 3);
    EXPECT_EQ(ids[0], 1);
    EXPECT_EQ(ids[1], 2);
    EXPECT_EQ(ids[2], 3);
}

TEST_F(MacroStorageTest, ForEachPreservesDataTypes) {
    auto shortcutMacro = createShortcutMacro(1, "Shortcut", 0x02, 0x05);
    auto consumerMacro = createConsumerControlMacro(2, "Consumer", 0x00EA);

    macroStorage.write(shortcutMacro);
    macroStorage.write(consumerMacro);

    std::vector<MacroType> types;
    macroStorage.forEach([&](const Macro& m) {
        types.push_back(m.data->type);
    });

    EXPECT_EQ(types.size(), 2);
    EXPECT_EQ(types[0], MacroType::SHORTCUT);
    EXPECT_EQ(types[1], MacroType::CONSUMER_CONTROL);
}

// ============================================================================
// GetNumStored Tests
// ============================================================================

TEST_F(MacroStorageTest, GetNumStoredEmpty) {
    EXPECT_EQ(macroStorage.getNumStored(), 0);
}

TEST_F(MacroStorageTest, GetNumStoredAfterWrite) {
    auto macro = createShortcutMacro(1, "Test");
    macroStorage.write(macro);

    EXPECT_EQ(macroStorage.getNumStored(), 1);
}

TEST_F(MacroStorageTest, GetNumStoredCachesResult) {
    auto macro1 = createShortcutMacro(1, "Macro 1");
    auto macro2 = createShortcutMacro(2, "Macro 2");

    macroStorage.write(macro1);
    macroStorage.write(macro2);

    // First call populates cache
    EXPECT_EQ(macroStorage.getNumStored(), 2);

    // Should still return cached value even if file changes
    EXPECT_EQ(macroStorage.getNumStored(), 2);
}

TEST_F(MacroStorageTest, GetNumStoredInvalidatesCacheOnWrite) {
    auto macro1 = createShortcutMacro(1, "Macro 1");
    EXPECT_EQ(macroStorage.getNumStored(), 0);

    macroStorage.write(macro1);
    EXPECT_EQ(macroStorage.getNumStored(), 1);
}

TEST_F(MacroStorageTest, GetNumStoredAfterRemove) {
    auto macro1 = createShortcutMacro(1, "Macro 1");
    auto macro2 = createShortcutMacro(2, "Macro 2");

    macroStorage.write(macro1);
    macroStorage.write(macro2);
    EXPECT_EQ(macroStorage.getNumStored(), 2);

    macroStorage.remove(1);
    EXPECT_EQ(macroStorage.getNumStored(), 1);
}

// ============================================================================
// Observable Tests
// ============================================================================

// Mock observer for MacroSaved events
class MacroSavedObserver : public tfw::utils::observables::Observer<MacroSaved> {
public:
    std::vector<MacroSaved> received_events;
    int call_count = 0;

    void observe(const MacroSaved& event) override {
        received_events.push_back(event);
        call_count++;
    }
};

// Mock observer for MacroRemoved events
class MacroRemovedObserver : public tfw::utils::observables::Observer<MacroRemoved> {
public:
    std::vector<MacroRemoved> received_events;
    int call_count = 0;

    void observe(const MacroRemoved& event) override {
        received_events.push_back(event);
        call_count++;
    }
};

TEST_F(MacroStorageTest, OnMacroSavedNotification) {
    auto macro = createShortcutMacro(1, "Test");

    MacroSavedObserver observer;
    macroStorage.onMacroSaved().addObserver(&observer);

    macroStorage.write(macro);
    EXPECT_EQ(observer.call_count, 1);
    EXPECT_EQ(observer.received_events[0].macroId, 1);
}

TEST_F(MacroStorageTest, OnMacroRemovedNotification) {
    auto macro = createShortcutMacro(1, "Test");
    macroStorage.write(macro);

    MacroRemovedObserver observer;
    macroStorage.onMacroRemoved().addObserver(&observer);

    macroStorage.remove(1);
    EXPECT_EQ(observer.call_count, 1);
    EXPECT_EQ(observer.received_events[0].macroId, 1);
}

TEST_F(MacroStorageTest, OnMacroSavedNotificationOnOverwrite) {
    auto macro1 = createShortcutMacro(1, "Original");
    auto macro2 = createShortcutMacro(1, "Updated");

    MacroSavedObserver observer;
    macroStorage.onMacroSaved().addObserver(&observer);

    macroStorage.write(macro1);
    EXPECT_EQ(observer.received_events[0].macroId, 1);

    macroStorage.write(macro2);
    EXPECT_EQ(observer.call_count, 2);
    EXPECT_EQ(observer.received_events[1].macroId, 1);
}

TEST_F(MacroStorageTest, MultipleObserversNotified) {
    auto macro = createShortcutMacro(1, "Test");

    MacroSavedObserver observer1, observer2;
    macroStorage.onMacroSaved().addObserver(&observer1);
    macroStorage.onMacroSaved().addObserver(&observer2);

    macroStorage.write(macro);
    EXPECT_EQ(observer1.call_count, 1);
    EXPECT_EQ(observer2.call_count, 1);
}

// ============================================================================
// Serialization/Deserialization Tests
// ============================================================================

TEST_F(MacroStorageTest, SerializedFormatContainsAllParts) {
    auto macro = createShortcutMacro(5, "MyMacro", 0x02, 0x0A);
    macroStorage.write(macro);

    std::string content = mockFs.getFileContent(testFilePath);
    // Format: id:name:type:data
    EXPECT_EQ(content.find("5:"), 0);  // ID at start
    EXPECT_NE(content.find("MyMacro"), std::string::npos);  // Name somewhere
    EXPECT_NE(content.find("0x01"), std::string::npos);  // Type (SHORTCUT) somewhere
}

TEST_F(MacroStorageTest, RoundTripShortcutMacro) {
    auto originalMacro = createShortcutMacro(1, "CtrlC", 0x01, 0x06);
    macroStorage.write(originalMacro);

    int foundCount = 0;
    macroStorage.forEach([&](const Macro& m) {
        if (m.data->id == 1) {
            foundCount++;
        }
    });

    EXPECT_EQ(foundCount, 1);
}

TEST_F(MacroStorageTest, RoundTripConsumerControlMacro) {
    auto originalMacro = createConsumerControlMacro(2, "Mute", 0x00E2);
    macroStorage.write(originalMacro);

    int foundCount = 0;
    macroStorage.forEach([&](const Macro& m) {
        if (m.data->id == 2) {
            foundCount++;
        }
    });

    EXPECT_EQ(foundCount, 1);
}

TEST_F(MacroStorageTest, RoundTripTextMacro) {
    auto originalMacro = createTextMacro(3, "Email", "email@test.com");
    macroStorage.write(originalMacro);

    int foundCount = 0;
    macroStorage.forEach([&](const Macro& m) {
        if (m.data->id == 3) {
            foundCount++;
        }
    });

    EXPECT_EQ(foundCount, 1);
}

// ============================================================================
// Edge Cases and Error Handling
// ============================================================================

TEST_F(MacroStorageTest, WriteAfterRemove) {
    auto macro1 = createShortcutMacro(1, "First");
    auto macro2 = createShortcutMacro(2, "Second");

    macroStorage.write(macro1);
    macroStorage.remove(1);
    macroStorage.write(macro2);

    EXPECT_EQ(macroStorage.getNumStored(), 1);
}

TEST_F(MacroStorageTest, RemoveAndRecreateWithSameId) {
    auto macro1 = createShortcutMacro(1, "First");
    auto macro2 = createShortcutMacro(1, "Second");

    macroStorage.write(macro1);
    macroStorage.remove(1);
    macroStorage.write(macro2);

    EXPECT_EQ(macroStorage.getNumStored(), 1);

    std::string retrievedName;
    macroStorage.forEach([&](const Macro& m) {
        if (m.data->id == 1) {
            retrievedName = std::string(m.name);
        }
    });
    EXPECT_EQ(retrievedName, "Second");
}

TEST_F(MacroStorageTest, LargeMacroId) {
    auto macro = createShortcutMacro(65535, "MaxId");  // Max uint16_t

    EXPECT_EQ(macroStorage.write(macro), 0);
    EXPECT_EQ(macro.data->id, 65535);
}

TEST_F(MacroStorageTest, MacroNameWithSpaces) {
    auto macro = createShortcutMacro(1, "Test With Spaces");

    // Should still be able to write and read
    EXPECT_EQ(macroStorage.write(macro), 0);

    std::string retrievedName;
    macroStorage.forEach([&](const Macro& m) {
        retrievedName = std::string(m.name);
    });
    EXPECT_EQ(retrievedName, "Test With Spaces");
}

TEST_F(MacroStorageTest, MixedMacroTypes) {
    auto shortcut = createShortcutMacro(1, "Shortcut");
    auto consumer = createConsumerControlMacro(2, "Consumer");
    auto system = createSystemControlMacro(3, "System");
    auto text = createTextMacro(4, "Text", "content");

    macroStorage.write(shortcut);
    macroStorage.write(consumer);
    macroStorage.write(system);
    macroStorage.write(text);

    EXPECT_EQ(macroStorage.getNumStored(), 4);

    std::vector<MacroType> types;
    macroStorage.forEach([&](const Macro& m) {
        types.push_back(m.data->type);
    });

    EXPECT_EQ(types[0], MacroType::SHORTCUT);
    EXPECT_EQ(types[1], MacroType::CONSUMER_CONTROL);
    EXPECT_EQ(types[2], MacroType::SYSTEM_CONTROL);
    EXPECT_EQ(types[3], MacroType::TEXT);
}

TEST_F(MacroStorageTest, IdAssignmentSkipsGaps) {
    auto macro1 = createShortcutMacro(0, "M1");
    auto macro2 = createShortcutMacro(0, "M2");

    macroStorage.write(macro1);
    EXPECT_EQ(macro1.data->id, 1);

    macroStorage.write(macro2);
    EXPECT_EQ(macro2.data->id, 2);
}

TEST_F(MacroStorageTest, IdAssignmentUsesHighestId) {
    auto macro1 = createShortcutMacro(1, "M1");
    auto macro2 = createShortcutMacro(5, "M2");
    auto macro3 = createShortcutMacro(0, "M3");  // Should get ID 6

    macroStorage.write(macro1);
    macroStorage.write(macro2);
    macroStorage.write(macro3);

    EXPECT_EQ(macro3.data->id, 6);
}
