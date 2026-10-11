#pragma once

#include <cstdint>

#include "tfw/hal/time/Clock.h"
#include "tfw/utils/ring_queue.h"

#include "HostComputer.h"
#include "ModifierHolder.h"
#include "USBAction.h"

namespace tfw::hal::usb {
    // Sends actions to the host computer as key presses, mouse events and so on. Characters of type actions are typed
    // one at a time by update(), with the typing delay in between, and modifiers of mouse actions and shortcuts that
    // hold them stay held between consecutive actions.
    class ActionSender {
    public:
        ActionSender(HostComputer& hostComputer, const time::Clock& clock);

        void send(Action& action);

        // Types queued characters and releases modifiers whose hold time has passed. Called continuously.
        void update();

        // Whether characters of type actions are still waiting to be typed
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

        HostComputer& hostComputer;
        const time::Clock& clock;
        ModifierHolder modifierHolder;

        utils::ring_queue<char> characterOutputQueue{1000}; // enforce size limit in storage and UI also
        uint64_t lastKeyboardWriteTime = 0;
        uint64_t typingDelayUs = 1'000;
    };
}
