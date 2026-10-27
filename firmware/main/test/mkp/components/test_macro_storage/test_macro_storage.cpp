#include <gtest/gtest.h>
#include <tfw/hal/fs/MockFileSystem.h>
#include "mkp/components/macros/MacroStorage.h"
#include <string>
#include <vector>
#include <memory>

#include "mkp/components/macros/MacroStorage.cpp"
#include "mkp/components/macros/MacroDataSerializers.cpp"
#include "mkp/components/generators/RandomTextGenerator.cpp"
#include "mkp/components/generators/UuidV4Generator.cpp"

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
    EXPECT_NE(content.find("1:Scroll Down:0x05:m=0x00:w=-3:p=0"), std::string::npos);
}

TEST_F(MacroStorageTest, WriteMouseButtonMacro) {
    auto macro = createMouseButtonMacro(1, "Middle Click", 0x04);

    EXPECT_EQ(macroStorage.write(macro), 0);

    std::string content = mockFs.getFileContent(testFilePath);
    EXPECT_NE(content.find("1:Middle Click:0x06:m=0x00:b=0x04"), std::string::npos);
}

TEST_F(MacroStorageTest, WriteMouseMovementMacro) {
    auto macro = createMouseMovementMacro(1, "Nudge", 10, -20);

    EXPECT_EQ(macroStorage.write(macro), 0);

    std::string content = mockFs.getFileContent(testFilePath);
    EXPECT_NE(content.find("1:Nudge:0x07:m=0x00:x=10:y=-20"), std::string::npos);
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
    mockFs.setFileContent(testFilePath, "1:Scroll:0x05:m=0x00:w=500:p=-500\n2:Movement:0x07:m=0x00:x=-40000:y=40000\n");

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

TEST_F(MacroStorageTest, WriteMouseMacroWithModifiers) {
    auto macro = createMouseScrollMacro(1, "Ctrl Scroll", 1, 0, 0x01);

    EXPECT_EQ(macroStorage.write(macro), 0);

    std::string content = mockFs.getFileContent(testFilePath);
    EXPECT_NE(content.find("1:Ctrl Scroll:0x05:m=0x01:w=1:p=0"), std::string::npos);
}

TEST_F(MacroStorageTest, WriteMidiNoteMacro) {
    auto macro = createMidiNoteMacro(1, "Middle C", 10, 60, 100, 250);

    EXPECT_EQ(macroStorage.write(macro), 0);

    std::string content = mockFs.getFileContent(testFilePath);
    EXPECT_NE(content.find("1:Middle C:0x09:c=10:n=60:v=100:d=250:r=0"), std::string::npos);
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
    mockFs.setFileContent(testFilePath, "1:Low:0x09:c=0:n=60:v=0:d=100:r=0\n2:High:0x09:c=17:n=200:v=200:d=100000:r=0\n");

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

TEST_F(MacroStorageTest, WriteRelativeControlChangeMacro) {
    auto macro = createRelativeControlChangeMacro(1, "Volume Down", 3, 7, -2, RelativeControlChangeEncoding::SIGN_MAGNITUDE);

    EXPECT_EQ(macroStorage.write(macro), 0);

    std::string content = mockFs.getFileContent(testFilePath);
    EXPECT_NE(content.find("1:Volume Down:0x0a:c=3:n=7:a=-2:e=2"), std::string::npos);
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
    mockFs.setFileContent(testFilePath, "1:Low:0x0a:c=0:n=7:a=-100:e=9\n2:High:0x0a:c=20:n=127:a=100:e=1\n");

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

TEST_F(MacroStorageTest, WriteAbsoluteControlChangeMacro) {
    auto macro = createAbsoluteControlChangeMacro(1, "Volume Down", 3, 7, -5);

    EXPECT_EQ(macroStorage.write(macro), 0);

    std::string content = mockFs.getFileContent(testFilePath);
    EXPECT_NE(content.find("1:Volume Down:0x0b:c=3:n=7:a=-5"), std::string::npos);
}

TEST_F(MacroStorageTest, AbsoluteControlChangeMacroClampsOutOfRangeValues) {
    mockFs.setFileContent(testFilePath, "1:Low:0x0b:c=0:n=7:a=-500\n2:High:0x0b:c=20:n=127:a=500\n");

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
    EXPECT_NE(content.find("1:Switch Window:0x01:m=0x04:k=0x2b:h=1:t=0"), std::string::npos);
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
    EXPECT_NE(content.find("1:Switch Window:0x01:m=0x04:k=0x2b:h=1:t=800"), std::string::npos);

    macroStorage.forEach([&](const Macro& m) {
        const auto& data = dynamic_cast<const ShortcutMacroData&>(*m.data);
        EXPECT_TRUE(data.holdModifiers);
        EXPECT_EQ(data.holdTimeMs, 800);
    });
}

TEST_F(MacroStorageTest, ShortcutMacroHoldTimeIsLimitedLikeSetting) {
    mockFs.setFileContent(testFilePath, "1:Switch Window:0x01:m=0x04:k=0x2b:h=1:t=999999\n");

    macroStorage.forEach([&](const Macro& m) {
        const auto& data = dynamic_cast<const ShortcutMacroData&>(*m.data);
        EXPECT_EQ(data.holdTimeMs, 5000);
    });
}

TEST_F(MacroStorageTest, UnparseableLinesAreIgnored) {
    mockFs.setFileContent(
        testFilePath,
        "1:Broken\n"
        "2:Unknown:0x7f:z=1\n"
        "3:No Keys:0x02:0x00e9\n"
        "4:Sleep:0x03:c=0x82\n"
        "5:Mixed Keys:0x04:t=a:b\n"
        "6:Text:0x04:t=content\n"
    );

    EXPECT_EQ(macroStorage.getNumStored(), 2);

    std::vector<std::string> names;
    macroStorage.forEach([&](const Macro& m) {
        names.emplace_back(m.name);
    });
    EXPECT_EQ(names, (std::vector<std::string>{"Sleep", "Text"}));
}

TEST_F(MacroStorageTest, WritingDropsLinesThatCantBeMacros) {
    mockFs.setFileContent(testFilePath, "1:Broken\n2:Sleep:0x03:c=0x82\n");

    auto macro = createTextMacro(3, "Text", "content");
    macroStorage.write(macro);

    EXPECT_EQ(macroStorage.getNumStored(), 2);
    EXPECT_EQ(mockFs.getFileContent(testFilePath).find("Broken"), std::string::npos);
}

TEST_F(MacroStorageTest, WritingKeepsMacrosThatCantBeParsed) {
    mockFs.setFileContent(testFilePath, "4:Later:0x7f:z=1\n");

    auto macro = createTextMacro(0, "Text", "content");
    macroStorage.write(macro);

    // The ID of the unparsed macro is still taken
    EXPECT_EQ(macro.data->id, 5);
    EXPECT_NE(mockFs.getFileContent(testFilePath).find("4:Later:0x7f:z=1\n"), std::string::npos);

    macroStorage.remove(5);
    EXPECT_NE(mockFs.getFileContent(testFilePath).find("4:Later:0x7f:z=1\n"), std::string::npos);
}

TEST_F(MacroStorageTest, UnparsedMacrosCanBeOverwrittenAndRemoved) {
    mockFs.setFileContent(testFilePath, "1:Later:0x7f:z=1\n2:Later:0x7f:z=1\n");

    auto macro = createTextMacro(1, "Text", "content");
    macroStorage.write(macro);
    macroStorage.remove(2);

    EXPECT_EQ(mockFs.getFileContent(testFilePath), "1:Text:0x04:t=content\n");
}

// ============================================================================
// Field Compatibility Tests
// ============================================================================

TEST_F(MacroStorageTest, MacrosWithoutKeysAreIgnored) {
    mockFs.setFileContent(testFilePath, "1:Copy:0x01:0x01:0x06:0:0\n2:Note:0x09:1:60:100:250:1\n");

    EXPECT_EQ(macroStorage.getNumStored(), 0);
}

TEST_F(MacroStorageTest, MacrosWithoutFieldsTakeDefaults) {
    mockFs.setFileContent(testFilePath, "1:Volume Up:0x02\n");

    int found = 0;
    macroStorage.forEach([&](const Macro& m) {
        EXPECT_EQ(dynamic_cast<const ConsumerControlMacroData&>(*m.data).usageId, 0);
        found++;
    });
    EXPECT_EQ(found, 1);
}

TEST_F(MacroStorageTest, UnknownFieldsAreSkipped) {
    mockFs.setFileContent(testFilePath, "1:Copy:0x01:z=9:k=0x06:m=0x01:h=1:t=200:future=abc\n");

    int found = 0;
    macroStorage.forEach([&](const Macro& m) {
        const auto& data = dynamic_cast<const ShortcutMacroData&>(*m.data);
        EXPECT_EQ(data.modifiers, 0x01);
        EXPECT_EQ(data.hidKeyCode, 0x06);
        EXPECT_TRUE(data.holdModifiers);
        EXPECT_EQ(data.holdTimeMs, 200);
        found++;
    });
    EXPECT_EQ(found, 1);
}

TEST_F(MacroStorageTest, MissingFieldsTakeDefaults) {
    mockFs.setFileContent(testFilePath, "1:Copy:0x01:k=0x06\n2:Note:0x09:n=64\n");

    int found = 0;
    macroStorage.forEach([&](const Macro& m) {
        if (m.data->id == 1) {
            const auto& data = dynamic_cast<const ShortcutMacroData&>(*m.data);
            EXPECT_EQ(data.modifiers, 0x00);
            EXPECT_EQ(data.hidKeyCode, 0x06);
            EXPECT_FALSE(data.holdModifiers);
            EXPECT_EQ(data.holdTimeMs, 0);
        } else {
            const auto& data = dynamic_cast<const MidiNoteMacroData&>(*m.data);
            EXPECT_EQ(data.channel, 1);
            EXPECT_EQ(data.note, 64);
            EXPECT_EQ(data.velocity, 127);
            EXPECT_FALSE(data.endOnRelease);
        }
        found++;
    });
    EXPECT_EQ(found, 2);
}

TEST_F(MacroStorageTest, MacrosMixingFieldsWithAndWithoutKeysAreIgnored) {
    mockFs.setFileContent(testFilePath, "1:Copy:0x01:m=0x01:0x06:h=0:t=0\n");

    EXPECT_EQ(macroStorage.getNumStored(), 0);
}

TEST_F(MacroStorageTest, RoundTripMacroTypesWithKeys) {
    auto text = createTextMacro(1, "Text", "a%3Db");
    auto consumer = createConsumerControlMacro(2, "Consumer", 0x00E9);
    auto absolute = createAbsoluteControlChangeMacro(3, "CC", 2, 10, -3);
    auto random = Macro{"UUID", std::make_shared<RandomTextMacroData>(4, RandomTextFormat::UUID_V4, true)};
    macroStorage.write(text);
    macroStorage.write(consumer);
    macroStorage.write(absolute);
    macroStorage.write(random);

    int found = 0;
    macroStorage.forEach([&](const Macro& m) {
        if (m.data->id == 1) {
            EXPECT_EQ(dynamic_cast<const TextMacroData&>(*m.data).text, "a%3Db");
        } else if (m.data->id == 2) {
            EXPECT_EQ(dynamic_cast<const ConsumerControlMacroData&>(*m.data).usageId, 0x00E9);
        } else if (m.data->id == 3) {
            const auto& data = dynamic_cast<const MidiAbsoluteControlChangeMacroData&>(*m.data);
            EXPECT_EQ(data.channel, 2);
            EXPECT_EQ(data.controller, 10);
            EXPECT_EQ(data.amount, -3);
        } else {
            EXPECT_TRUE(dynamic_cast<const RandomTextMacroData&>(*m.data).quoted);
        }
        found++;
    });
    EXPECT_EQ(found, 4);
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
    EXPECT_NE(content.find("1:Login:0x08:s=m3/d0;m4/d250"), std::string::npos);
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
    mockFs.setFileContent(testFilePath, "1:Slow:0x08:s=m3/d999999\n");

    const auto steps = readSequenceSteps(1);

    ASSERT_EQ(steps.size(), 1);
    EXPECT_EQ(steps[0].delayMs, MaxSequenceStepDelayMs);
}

TEST_F(MacroStorageTest, SequenceMacroStepsSkipUnknownFieldsAndDefaultMissingOnes) {
    mockFs.setFileContent(testFilePath, "1:Sequence:0x08:s=m3/x7;m4/d250/x7;l0/r2/x7\n");

    const auto steps = readSequenceSteps(1);

    ASSERT_EQ(steps.size(), 3);
    EXPECT_EQ(steps[0].macroId, 3);
    EXPECT_EQ(steps[0].delayMs, 0);
    EXPECT_EQ(steps[1].delayMs, 250);
    EXPECT_TRUE(steps[2].isLoop());
    EXPECT_EQ(steps[2].loopRepeats, 2);
}

TEST_F(MacroStorageTest, SequenceMacrosWithoutKeysAreIgnored) {
    mockFs.setFileContent(testFilePath, "1:Login:0x08:3;0;4;250\n");

    EXPECT_EQ(macroStorage.getNumStored(), 0);
}

TEST_F(MacroStorageTest, WriteSequenceMacroWithLoop) {
    auto macro = createSequenceMacro(1, "Repeat", {{3, 0}, {4, 250}, {0, 500, 0, 3}});

    EXPECT_EQ(macroStorage.write(macro), 0);

    std::string content = mockFs.getFileContent(testFilePath);
    EXPECT_NE(content.find("1:Repeat:0x08:s=m3/d0;m4/d250;l0/r3/d500"), std::string::npos);
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
    mockFs.setFileContent(testFilePath, "1:Many:0x08:s=m3/d0;l0/r999999/d0\n");

    const auto steps = readSequenceSteps(1);

    ASSERT_EQ(steps.size(), 2);
    EXPECT_EQ(steps[1].loopRepeats, MaxSequenceLoopRepeats);
}

TEST_F(MacroStorageTest, InvalidSequenceLoopsAreIgnored) {
    mockFs.setFileContent(
        testFilePath,
        "1:Loop First:0x08:s=l0/r1/d0\n"
        "2:Loop To Itself:0x08:s=m3/d0;l1/r1/d0\n"
        "3:Loop Forward:0x08:s=m3/d0;l2/r1/d0;m4/d0\n"
        "4:No Repeats:0x08:s=m3/d0;l0/r0/d0\n"
        "5:Missing Repeats:0x08:s=m3/d0;l0/d0\n"
        "6:Missing Target:0x08:s=m3/d0;l/r1/d0\n"
        "7:Macro And Loop:0x08:s=m3/d0;m4/l0/r1/d0\n"
    );

    EXPECT_EQ(macroStorage.getNumStored(), 0);
}

TEST_F(MacroStorageTest, InvalidSequenceMacrosAreIgnored) {
    std::string tooManySteps = "6:Too Many:0x08:s=";
    for (size_t i = 0; i <= MaxSequenceSteps; i++) {
        tooManySteps += i == 0 ? "m1/d0" : ";m1/d0";
    }

    mockFs.setFileContent(
        testFilePath,
        "1:No Steps:0x08:s=\n"
        "2:Neither Macro Nor Loop:0x08:s=m3/d0;d250\n"
        "3:Empty Step:0x08:s=m3/d0;;m4/d0\n"
        "4:Empty Field:0x08:s=m3/d0/;m4/d0\n"
        "5:Missing Steps:0x08\n" +
        tooManySteps + "\n"
    );

    EXPECT_EQ(macroStorage.getNumStored(), 0);
}

TEST_F(MacroStorageTest, WriteRandomTextMacro) {
    auto macro = Macro{"UUID", std::make_shared<RandomTextMacroData>(1, RandomTextFormat::UUID_V4, true)};

    EXPECT_EQ(macroStorage.write(macro), 0);

    std::string content = mockFs.getFileContent(testFilePath);
    EXPECT_NE(content.find("1:UUID:0x0c:f=1:q=1"), std::string::npos);
}

TEST_F(MacroStorageTest, ReadRandomTextMacro) {
    mockFs.setFileContent(testFilePath, "1:UUID:0x0c:f=1:q=0\n2:Quoted UUID:0x0c:f=1:q=1\n");

    int found = 0;
    macroStorage.forEach([&](const Macro& m) {
        const auto& data = dynamic_cast<const RandomTextMacroData&>(*m.data);
        EXPECT_EQ(data.format, RandomTextFormat::UUID_V4);
        EXPECT_EQ(data.quoted, m.data->id == 2);
        found++;
    });

    EXPECT_EQ(found, 2);
}

TEST_F(MacroStorageTest, RandomTextMacroWithUnknownFormatIsIgnored) {
    mockFs.setFileContent(testFilePath, "1:Unknown:0x0c:f=0:q=0\n2:Later:0x0c:f=99:q=0\n3:Missing Format:0x0c:q=1\n");

    EXPECT_EQ(macroStorage.getNumStored(), 0);
}
