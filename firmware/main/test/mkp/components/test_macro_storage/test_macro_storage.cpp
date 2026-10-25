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
        uint8_t hidKeyCode = 0x04,
        bool holdModifiers = false,
        uint32_t holdTimeMs = 0) {
        auto data = std::make_shared<ShortcutMacroData>(id, modifiers, hidKeyCode, holdModifiers, holdTimeMs);
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

    Macro createMouseScrollMacro(
        uint16_t id,
        const std::string_view& name,
        int8_t wheel = 0,
        int8_t pan = 0,
        uint8_t modifiers = 0x00) {
        auto data = std::make_shared<MouseScrollMacroData>(id, modifiers, wheel, pan);
        return Macro{name, data};
    }

    Macro createMouseButtonMacro(
        uint16_t id,
        const std::string_view& name,
        uint8_t buttons = 0x01,
        uint8_t modifiers = 0x00) {
        auto data = std::make_shared<MouseButtonMacroData>(id, modifiers, buttons);
        return Macro{name, data};
    }

    Macro createMouseMovementMacro(
        uint16_t id,
        const std::string_view& name,
        int16_t x = 0,
        int16_t y = 0,
        uint8_t modifiers = 0x00) {
        auto data = std::make_shared<MouseMovementMacroData>(id, modifiers, x, y);
        return Macro{name, data};
    }

    Macro createMidiNoteMacro(
        uint16_t id,
        const std::string_view& name,
        uint8_t channel = 1,
        uint8_t note = 60,
        uint8_t velocity = 100,
        uint32_t durationMs = 100,
        bool endOnRelease = false) {
        auto data = std::make_shared<MidiNoteMacroData>(id, channel, note, velocity, durationMs, endOnRelease);
        return Macro{name, data};
    }

    Macro createRelativeControlChangeMacro(
        uint16_t id,
        const std::string_view& name,
        uint8_t channel = 1,
        uint8_t controller = 7,
        int8_t amount = 1,
        RelativeControlChangeEncoding encoding = RelativeControlChangeEncoding::BINARY_OFFSET) {
        auto data = std::make_shared<MidiRelativeControlChangeMacroData>(id, channel, controller, amount, encoding);
        return Macro{name, data};
    }

    Macro createAbsoluteControlChangeMacro(
        uint16_t id,
        const std::string_view& name,
        uint8_t channel = 1,
        uint8_t controller = 7,
        int8_t amount = 1) {
        auto data = std::make_shared<MidiAbsoluteControlChangeMacroData>(id, channel, controller, amount);
        return Macro{name, data};
    }

    Macro createSequenceMacro(
        uint16_t id,
        const std::string_view& name,
        std::vector<SequenceStep> steps) {
        auto data = std::make_shared<SequenceMacroData>(id, std::move(steps));
        return Macro{name, data};
    }

    std::vector<SequenceStep> readSequenceSteps(uint16_t id) {
        std::vector<SequenceStep> steps;
        macroStorage.forEach([&](const Macro& m) {
            if (m.data->id == id && m.data->type == MacroType::SEQUENCE) {
                steps = dynamic_cast<const SequenceMacroData&>(*m.data).steps;
            }
        });
        return steps;
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

TEST_F(MacroStorageTest, WriteMouseScrollMacro) {
    auto macro = createMouseScrollMacro(1, "Scroll Down", -3, 0);

    EXPECT_EQ(macroStorage.write(macro), 0);

    std::string content = mockFs.getFileContent(testFilePath);
    EXPECT_NE(content.find("1:Scroll Down:0x05:0x00:-3:0"), std::string::npos);
}

TEST_F(MacroStorageTest, WriteMouseButtonMacro) {
    auto macro = createMouseButtonMacro(1, "Middle Click", 0x04);

    EXPECT_EQ(macroStorage.write(macro), 0);

    std::string content = mockFs.getFileContent(testFilePath);
    EXPECT_NE(content.find("1:Middle Click:0x06:0x00:0x04"), std::string::npos);
}

TEST_F(MacroStorageTest, WriteMouseMovementMacro) {
    auto macro = createMouseMovementMacro(1, "Nudge", 10, -20);

    EXPECT_EQ(macroStorage.write(macro), 0);

    std::string content = mockFs.getFileContent(testFilePath);
    EXPECT_NE(content.find("1:Nudge:0x07:0x00:10:-20"), std::string::npos);
}

TEST_F(MacroStorageTest, RoundTripMouseMacros) {
    auto scroll = createMouseScrollMacro(1, "Scroll", 5, -2, 0x01);
    auto button = createMouseButtonMacro(2, "Button", 0x03, 0x02);
    auto movement = createMouseMovementMacro(3, "Movement", -1000, 20000, 0x84);
    macroStorage.write(scroll);
    macroStorage.write(button);
    macroStorage.write(movement);

    int found = 0;
    macroStorage.forEach([&](const Macro& m) {
        if (m.data->type == MacroType::MOUSE_SCROLL) {
            const auto& data = dynamic_cast<const MouseScrollMacroData&>(*m.data);
            EXPECT_EQ(data.modifiers, 0x01);
            EXPECT_EQ(data.wheel, 5);
            EXPECT_EQ(data.pan, -2);
        } else if (m.data->type == MacroType::MOUSE_BUTTON) {
            const auto& data = dynamic_cast<const MouseButtonMacroData&>(*m.data);
            EXPECT_EQ(data.modifiers, 0x02);
            EXPECT_EQ(data.buttons, 0x03);
        } else if (m.data->type == MacroType::MOUSE_MOVEMENT) {
            const auto& data = dynamic_cast<const MouseMovementMacroData&>(*m.data);
            EXPECT_EQ(data.modifiers, 0x84);
            EXPECT_EQ(data.x, -1000);
            EXPECT_EQ(data.y, 20000);
        }
        found++;
    });
    EXPECT_EQ(found, 3);
}

TEST_F(MacroStorageTest, MouseMacrosClampOutOfRangeValues) {
    mockFs.setFileContent(testFilePath, "1:Scroll:0x05:0x00:500:-500\n2:Movement:0x07:0x00:-40000:40000\n");

    macroStorage.forEach([&](const Macro& m) {
        if (m.data->type == MacroType::MOUSE_SCROLL) {
            const auto& data = dynamic_cast<const MouseScrollMacroData&>(*m.data);
            EXPECT_EQ(data.wheel, 127);
            EXPECT_EQ(data.pan, -127);
        } else {
            const auto& data = dynamic_cast<const MouseMovementMacroData&>(*m.data);
            EXPECT_EQ(data.x, -32767);
            EXPECT_EQ(data.y, 32767);
        }
    });
}

TEST_F(MacroStorageTest, MouseMacrosWithWrongNumberOfPartsAreIgnored) {
    mockFs.setFileContent(
        testFilePath,
        "1:Scroll:0x05:-3:0\n2:Button:0x06:0x04\n3:Movement:0x07:10:-20\n4:Scroll:0x05:0x00:1:0:0\n"
    );

    EXPECT_EQ(macroStorage.getNumStored(), 0);
}

TEST_F(MacroStorageTest, WriteMouseMacroWithModifiers) {
    auto macro = createMouseScrollMacro(1, "Ctrl Scroll", 1, 0, 0x01);

    EXPECT_EQ(macroStorage.write(macro), 0);

    std::string content = mockFs.getFileContent(testFilePath);
    EXPECT_NE(content.find("1:Ctrl Scroll:0x05:0x01:1:0"), std::string::npos);
}

TEST_F(MacroStorageTest, WriteMidiNoteMacro) {
    auto macro = createMidiNoteMacro(1, "Middle C", 10, 60, 100, 250);

    EXPECT_EQ(macroStorage.write(macro), 0);

    std::string content = mockFs.getFileContent(testFilePath);
    EXPECT_NE(content.find("1:Middle C:0x09:10:60:100:250:0"), std::string::npos);
}

TEST_F(MacroStorageTest, RoundTripMidiNoteMacro) {
    auto macro = createMidiNoteMacro(1, "Note", 16, 127, 1, 60000);
    auto releasedMacro = createMidiNoteMacro(2, "Released", 1, 0, 127, 0, true);
    macroStorage.write(macro);
    macroStorage.write(releasedMacro);

    int found = 0;
    macroStorage.forEach([&](const Macro& m) {
        const auto& data = dynamic_cast<const MidiNoteMacroData&>(*m.data);
        if (m.data->id == 1) {
            EXPECT_EQ(data.channel, 16);
            EXPECT_EQ(data.note, 127);
            EXPECT_EQ(data.velocity, 1);
            EXPECT_EQ(data.durationMs, 60000);
            EXPECT_FALSE(data.endOnRelease);
        } else {
            EXPECT_EQ(data.note, 0);
            EXPECT_TRUE(data.endOnRelease);
        }
        found++;
    });
    EXPECT_EQ(found, 2);
}

TEST_F(MacroStorageTest, MidiNoteMacroClampsOutOfRangeValues) {
    mockFs.setFileContent(testFilePath, "1:Low:0x09:0:60:0:100:0\n2:High:0x09:17:200:200:100000:0\n");

    int found = 0;
    macroStorage.forEach([&](const Macro& m) {
        const auto& data = dynamic_cast<const MidiNoteMacroData&>(*m.data);
        if (m.data->id == 1) {
            EXPECT_EQ(data.channel, 1);
            EXPECT_EQ(data.note, 60);
            EXPECT_EQ(data.velocity, 1);
            EXPECT_EQ(data.durationMs, 100);
        } else {
            EXPECT_EQ(data.channel, 16);
            EXPECT_EQ(data.note, 127);
            EXPECT_EQ(data.velocity, 127);
            EXPECT_EQ(data.durationMs, 60000);
        }
        found++;
    });
    EXPECT_EQ(found, 2);
}

TEST_F(MacroStorageTest, MidiNoteMacroWithWrongNumberOfPartsIsIgnored) {
    mockFs.setFileContent(testFilePath, "1:Note:0x09:1:60:100:100\n2:Note:0x09:1:60:100:100:0:0\n");

    EXPECT_EQ(macroStorage.getNumStored(), 0);
}

TEST_F(MacroStorageTest, WriteRelativeControlChangeMacro) {
    auto macro = createRelativeControlChangeMacro(1, "Volume Down", 3, 7, -2, RelativeControlChangeEncoding::SIGN_MAGNITUDE);

    EXPECT_EQ(macroStorage.write(macro), 0);

    std::string content = mockFs.getFileContent(testFilePath);
    EXPECT_NE(content.find("1:Volume Down:0x0a:3:7:-2:2"), std::string::npos);
}

TEST_F(MacroStorageTest, RoundTripRelativeControlChangeMacro) {
    auto macro = createRelativeControlChangeMacro(1, "Pan", 16, 119, 63, RelativeControlChangeEncoding::TWOS_COMPLEMENT);
    macroStorage.write(macro);

    int found = 0;
    macroStorage.forEach([&](const Macro& m) {
        const auto& data = dynamic_cast<const MidiRelativeControlChangeMacroData&>(*m.data);
        EXPECT_EQ(data.channel, 16);
        EXPECT_EQ(data.controller, 119);
        EXPECT_EQ(data.amount, 63);
        EXPECT_EQ(data.encoding, RelativeControlChangeEncoding::TWOS_COMPLEMENT);
        found++;
    });
    EXPECT_EQ(found, 1);
}

TEST_F(MacroStorageTest, RelativeControlChangeMacroClampsOutOfRangeValues) {
    mockFs.setFileContent(testFilePath, "1:Low:0x0a:0:7:-100:9\n2:High:0x0a:20:127:100:1\n");

    int found = 0;
    macroStorage.forEach([&](const Macro& m) {
        const auto& data = dynamic_cast<const MidiRelativeControlChangeMacroData&>(*m.data);
        if (m.data->id == 1) {
            EXPECT_EQ(data.channel, 1);
            EXPECT_EQ(data.amount, -63);
            EXPECT_EQ(data.encoding, RelativeControlChangeEncoding::BINARY_OFFSET);
        } else {
            EXPECT_EQ(data.channel, 16);
            EXPECT_EQ(data.controller, 119);
            EXPECT_EQ(data.amount, 63);
        }
        found++;
    });
    EXPECT_EQ(found, 2);
}

TEST_F(MacroStorageTest, RelativeControlChangeMacroWithWrongNumberOfPartsIsIgnored) {
    mockFs.setFileContent(testFilePath, "1:CC:0x0a:1:7:1\n2:CC:0x0a:1:7:1:0:0\n");

    EXPECT_EQ(macroStorage.getNumStored(), 0);
}

TEST_F(MacroStorageTest, WriteAbsoluteControlChangeMacro) {
    auto macro = createAbsoluteControlChangeMacro(1, "Volume Down", 3, 7, -5);

    EXPECT_EQ(macroStorage.write(macro), 0);

    std::string content = mockFs.getFileContent(testFilePath);
    EXPECT_NE(content.find("1:Volume Down:0x0b:3:7:-5"), std::string::npos);
}

TEST_F(MacroStorageTest, AbsoluteControlChangeMacroClampsOutOfRangeValues) {
    mockFs.setFileContent(testFilePath, "1:Low:0x0b:0:7:-500\n2:High:0x0b:20:127:500\n");

    int found = 0;
    macroStorage.forEach([&](const Macro& m) {
        const auto& data = dynamic_cast<const MidiAbsoluteControlChangeMacroData&>(*m.data);
        if (m.data->id == 1) {
            EXPECT_EQ(data.channel, 1);
            EXPECT_EQ(data.controller, 7);
            EXPECT_EQ(data.amount, -127);
        } else {
            EXPECT_EQ(data.channel, 16);
            EXPECT_EQ(data.controller, 119);
            EXPECT_EQ(data.amount, 127);
        }
        found++;
    });
    EXPECT_EQ(found, 2);
}

TEST_F(MacroStorageTest, AbsoluteControlChangeMacroWithWrongNumberOfPartsIsIgnored) {
    mockFs.setFileContent(testFilePath, "1:CC:0x0b:1:7\n2:CC:0x0b:1:7:1:0\n");

    EXPECT_EQ(macroStorage.getNumStored(), 0);
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

TEST_F(MacroStorageTest, WriteShortcutMacroHoldingModifiers) {
    auto macro = createShortcutMacro(1, "Switch Window", 0x04, 0x2b, true);

    EXPECT_EQ(macroStorage.write(macro), 0);

    std::string content = mockFs.getFileContent(testFilePath);
    EXPECT_NE(content.find("1:Switch Window:0x01:0x04:0x2b:1:0"), std::string::npos);
}

TEST_F(MacroStorageTest, RoundTripShortcutMacroHoldFlag) {
    auto holding = createShortcutMacro(1, "Switch Window", 0x04, 0x2b, true);
    auto releasing = createShortcutMacro(2, "Copy", 0x01, 0x06, false);
    macroStorage.write(holding);
    macroStorage.write(releasing);

    macroStorage.forEach([&](const Macro& m) {
        const auto& data = dynamic_cast<const ShortcutMacroData&>(*m.data);
        EXPECT_EQ(data.holdModifiers, m.data->id == 1);
    });
}

TEST_F(MacroStorageTest, RoundTripShortcutMacroHoldTime) {
    auto macro = createShortcutMacro(1, "Switch Window", 0x04, 0x2b, true, 800);
    macroStorage.write(macro);

    std::string content = mockFs.getFileContent(testFilePath);
    EXPECT_NE(content.find("1:Switch Window:0x01:0x04:0x2b:1:800"), std::string::npos);

    macroStorage.forEach([&](const Macro& m) {
        const auto& data = dynamic_cast<const ShortcutMacroData&>(*m.data);
        EXPECT_TRUE(data.holdModifiers);
        EXPECT_EQ(data.holdTimeMs, 800);
    });
}

TEST_F(MacroStorageTest, ShortcutMacroHoldTimeIsLimitedLikeSetting) {
    mockFs.setFileContent(testFilePath, "1:Switch Window:0x01:0x04:0x2b:1:999999\n");

    macroStorage.forEach([&](const Macro& m) {
        const auto& data = dynamic_cast<const ShortcutMacroData&>(*m.data);
        EXPECT_EQ(data.holdTimeMs, 5000);
    });
}

TEST_F(MacroStorageTest, ShortcutMacroWithWrongNumberOfPartsIsIgnored) {
    mockFs.setFileContent(testFilePath, "1:Copy:0x01:0x01:0x06\n2:Copy:0x01:0x01:0x06:0\n");

    EXPECT_EQ(macroStorage.getNumStored(), 0);
}

TEST_F(MacroStorageTest, UnparseableLinesAreIgnored) {
    mockFs.setFileContent(
        testFilePath,
        "1:Broken\n"
        "2:Unknown:0x7f:1\n"
        "3:Volume Up:0x02\n"
        "4:Sleep:0x03:0x82\n"
        "5:Missing Text:0x04\n"
        "6:Text:0x04:content\n"
    );

    EXPECT_EQ(macroStorage.getNumStored(), 2);

    std::vector<std::string> names;
    macroStorage.forEach([&](const Macro& m) {
        names.emplace_back(m.name);
    });
    EXPECT_EQ(names, (std::vector<std::string>{"Sleep", "Text"}));
}

TEST_F(MacroStorageTest, WritingKeepsParseableMacrosOnly) {
    mockFs.setFileContent(testFilePath, "1:Broken\n2:Sleep:0x03:0x82\n");

    auto macro = createTextMacro(3, "Text", "content");
    macroStorage.write(macro);

    EXPECT_EQ(macroStorage.getNumStored(), 2);
    EXPECT_EQ(mockFs.getFileContent(testFilePath).find("Broken"), std::string::npos);
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
    auto scroll = createMouseScrollMacro(5, "Scroll", 1, 0);
    auto button = createMouseButtonMacro(6, "Button", 0x01);
    auto movement = createMouseMovementMacro(7, "Movement", 1, 1);

    macroStorage.write(shortcut);
    macroStorage.write(consumer);
    macroStorage.write(system);
    macroStorage.write(text);
    macroStorage.write(scroll);
    macroStorage.write(button);
    macroStorage.write(movement);

    EXPECT_EQ(macroStorage.getNumStored(), 7);

    std::vector<MacroType> types;
    macroStorage.forEach([&](const Macro& m) {
        types.push_back(m.data->type);
    });

    EXPECT_EQ(types[0], MacroType::SHORTCUT);
    EXPECT_EQ(types[1], MacroType::CONSUMER_CONTROL);
    EXPECT_EQ(types[2], MacroType::SYSTEM_CONTROL);
    EXPECT_EQ(types[3], MacroType::TEXT);
    EXPECT_EQ(types[4], MacroType::MOUSE_SCROLL);
    EXPECT_EQ(types[5], MacroType::MOUSE_BUTTON);
    EXPECT_EQ(types[6], MacroType::MOUSE_MOVEMENT);
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

// ============================================================================
// Sequence Tests
// ============================================================================

TEST_F(MacroStorageTest, WriteSequenceMacro) {
    auto macro = createSequenceMacro(1, "Login", {{3, 0}, {4, 250}});

    EXPECT_EQ(macroStorage.write(macro), 0);

    std::string content = mockFs.getFileContent(testFilePath);
    EXPECT_NE(content.find("1:Login:0x08:3;0;4;250"), std::string::npos);
}

TEST_F(MacroStorageTest, RoundTripSequenceMacro) {
    auto macro = createSequenceMacro(1, "Login", {{3, 0}, {4, 250}, {3, 1000}});
    macroStorage.write(macro);

    const auto steps = readSequenceSteps(1);

    ASSERT_EQ(steps.size(), 3);
    EXPECT_EQ(steps[0].macroId, 3);
    EXPECT_EQ(steps[0].delayMs, 0);
    EXPECT_EQ(steps[1].macroId, 4);
    EXPECT_EQ(steps[1].delayMs, 250);
    EXPECT_EQ(steps[2].macroId, 3);
    EXPECT_EQ(steps[2].delayMs, 1000);
}

TEST_F(MacroStorageTest, RoundTripSequenceMacroWithMostSteps) {
    std::vector<SequenceStep> steps;
    for (uint16_t i = 0; i < MaxSequenceSteps; i++) {
        steps.push_back({static_cast<uint16_t>(60000 + i), MaxSequenceStepDelayMs});
    }
    auto macro = createSequenceMacro(1, "Long", steps);
    macroStorage.write(macro);

    const auto readSteps = readSequenceSteps(1);

    ASSERT_EQ(readSteps.size(), MaxSequenceSteps);
    EXPECT_EQ(readSteps.back().macroId, 60000 + MaxSequenceSteps - 1);
    EXPECT_EQ(readSteps.back().delayMs, MaxSequenceStepDelayMs);
}

TEST_F(MacroStorageTest, SequenceMacroDelayIsLimited) {
    mockFs.setFileContent(testFilePath, "1:Slow:0x08:3;999999\n");

    const auto steps = readSequenceSteps(1);

    ASSERT_EQ(steps.size(), 1);
    EXPECT_EQ(steps[0].delayMs, MaxSequenceStepDelayMs);
}

TEST_F(MacroStorageTest, WriteSequenceMacroWithLoop) {
    auto macro = createSequenceMacro(1, "Repeat", {{3, 0}, {4, 250}, {0, 500, 0, 3}});

    EXPECT_EQ(macroStorage.write(macro), 0);

    std::string content = mockFs.getFileContent(testFilePath);
    EXPECT_NE(content.find("1:Repeat:0x08:3;0;4;250;L0;3;500"), std::string::npos);
}

TEST_F(MacroStorageTest, RoundTripSequenceMacroWithLoops) {
    auto macro = createSequenceMacro(1, "Repeat", {{3, 0}, {4, 250}, {0, 0, 1, 2}, {5, 10}, {0, 500, 0, 3}});
    macroStorage.write(macro);

    const auto steps = readSequenceSteps(1);

    ASSERT_EQ(steps.size(), 5);
    EXPECT_FALSE(steps[1].isLoop());
    EXPECT_EQ(steps[1].macroId, 4);
    EXPECT_TRUE(steps[2].isLoop());
    EXPECT_EQ(steps[2].loopTarget, 1);
    EXPECT_EQ(steps[2].loopRepeats, 2);
    EXPECT_EQ(steps[2].delayMs, 0);
    EXPECT_EQ(steps[3].macroId, 5);
    EXPECT_TRUE(steps[4].isLoop());
    EXPECT_EQ(steps[4].loopTarget, 0);
    EXPECT_EQ(steps[4].loopRepeats, 3);
    EXPECT_EQ(steps[4].delayMs, 500);
}

TEST_F(MacroStorageTest, SequenceMacroLoopRepeatsAreLimited) {
    mockFs.setFileContent(testFilePath, "1:Many:0x08:3;0;L0;999999;0\n");

    const auto steps = readSequenceSteps(1);

    ASSERT_EQ(steps.size(), 2);
    EXPECT_EQ(steps[1].loopRepeats, MaxSequenceLoopRepeats);
}

TEST_F(MacroStorageTest, InvalidSequenceLoopsAreIgnored) {
    mockFs.setFileContent(
        testFilePath,
        "1:Loop First:0x08:L0;1;0\n"
        "2:Loop To Itself:0x08:3;0;L1;1;0\n"
        "3:Loop Forward:0x08:3;0;L2;1;0;4;0\n"
        "4:No Repeats:0x08:3;0;L0;0;0\n"
        "5:Missing Delay:0x08:3;0;L0;1\n"
        "6:Missing Target:0x08:3;0;L;1;0\n"
    );

    EXPECT_EQ(macroStorage.getNumStored(), 0);
}

TEST_F(MacroStorageTest, InvalidSequenceMacrosAreIgnored) {
    std::string tooManySteps = "6:Too Many:0x08:";
    for (size_t i = 0; i <= MaxSequenceSteps; i++) {
        tooManySteps += i == 0 ? "1;0" : ";1;0";
    }

    mockFs.setFileContent(
        testFilePath,
        "1:No Steps:0x08:\n"
        "2:Missing Delay:0x08:3;0;4\n"
        "3:Empty Number:0x08:3;;4;0\n"
        "4:Extra Part:0x08:3;0:4;0\n"
        "5:Missing Part:0x08\n" +
        tooManySteps + "\n"
    );

    EXPECT_EQ(macroStorage.getNumStored(), 0);
}
