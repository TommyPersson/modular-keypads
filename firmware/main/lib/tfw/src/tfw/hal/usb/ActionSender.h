#pragma once

#include <cstdint>
#include <optional>
#include <vector>

#include "tfw/hal/time/Clock.h"
#include "tfw/utils/ring_queue.h"

#include "HostComputer.h"
#include "ModifierHolder.h"
#include "USBAction.h"

namespace tfw::hal::usb {
    // Sends actions to the host computer as key presses, mouse events and so on. Characters of type actions are typed
    // one at a time by update(), with the typing delay in between. Mouse movement beyond the range of a report is sent
    // one report per update(). Modifiers of mouse actions and shortcuts that hold them stay held between consecutive
    // actions. MIDI notes are ended by update() once their duration has passed, and held ones by note off actions.
    class ActionSender {
    public:
        ActionSender(HostComputer& hostComputer, const time::Clock& clock);

        void send(Action& action);

        // Types queued characters, sends remaining mouse movement, releases modifiers whose hold time has passed and
        // ends MIDI notes whose duration has passed. Called continuously.
        void update();

        // Whether characters of type actions or mouse movement are still waiting to be sent. Playing MIDI notes don't
        // count, so that sequences can play notes together.
        bool isBusy() const;

        // Time to wait between characters typed by type actions
        void setTypingDelayMs(uint32_t delayMs);

        // Time to keep modifiers held after the last action using them, for actions without a hold time of their own
        void setModifierHoldMs(uint32_t holdMs);

    private:
        void sendShortcut(const ShortcutAction& action);
        void sendConsumerControl(const ConsumerControlAction& action);
        void sendSystemControl(const SystemControlAction& action);
        void sendType(const TypeAction& action);
        void sendMouse(const MouseAction& action);
        void sendMouseMovementPart(int8_t wheel, int8_t pan);
        void sendMidiNote(const MidiNoteAction& action);
        void sendMidiNoteOff(const MidiNoteOffAction& action);
        void sendMidiControlChange(const MidiControlChangeAction& action);
        void endMidiNotes(uint64_t now);

        HostComputer& hostComputer;
        const time::Clock& clock;
        ModifierHolder modifierHolder;

        utils::ring_queue<char> characterOutputQueue{1000}; // enforce size limit in storage and UI also
        uint64_t lastKeyboardWriteTime = 0;
        uint64_t typingDelayUs = 1'000;

        // Movement not yet sent, with the modifiers of the latest mouse action, which stay held until it's sent
        int32_t remainingMouseX = 0;
        int32_t remainingMouseY = 0;
        uint8_t mouseModifiers = 0;

        struct PlayingMidiNote {
            uint8_t channel;
            uint8_t note;
            std::optional<uint64_t> endTimeUs; // None for held notes
        };

        std::vector<PlayingMidiNote> playingMidiNotes;
    };
}
