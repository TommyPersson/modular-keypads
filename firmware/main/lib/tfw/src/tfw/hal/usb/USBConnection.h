#pragma once

#include <cstdint>
#include <memory>
#include "USBAction.h"
#include "tfw/hal/time/Clock.h"

namespace tfw::hal::usb {
    class Connection {
    public:
        virtual ~Connection() = default;

        virtual void setup() = 0;
        virtual void update() = 0;
        virtual bool isConnected() = 0;

        virtual void sendAction(Action& action) = 0;

        // Whether earlier actions are still being sent, such as the characters of a type action being typed
        virtual bool isBusy() = 0;

        // Time to wait between characters typed by type actions
        virtual void setTypingDelayMs(uint32_t delayMs) = 0;

        // Time to keep keyboard modifiers of mouse actions, and of shortcut actions that hold them without a hold time of
        // their own, held after the last such action
        virtual void setModifierHoldMs(uint32_t holdMs) = 0;

        static std::unique_ptr<Connection> create(time::Clock& clock);
    };
}