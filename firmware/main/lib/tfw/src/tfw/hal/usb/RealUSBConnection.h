#pragma once

#include "ModifierHolder.h"
#include "USBConnection.h"
#include "tfw/utils/ring_queue.h"

namespace tfw::hal::usb {
    class RealConnection final : public Connection {
    public:
        void setup() override;
        void update() override;
        bool isConnected() override;
        void sendAction(Action& action) override;
        void setTypingDelayMs(uint32_t delayMs) override;
        void setModifierHoldMs(uint32_t holdMs) override;
    private:
        void applyModifierChange(const ModifierChange& change);

        utils::ring_queue<char> characterOutputQueue{1000}; // enforce size limit in storage and UI also
        uint64_t lastKeyboardWriteTime = 0;
        uint64_t typingDelayUs = 1'000;
        ModifierHolder modifierHolder;
    };
}
