#pragma once

#include <cstdint>
#include <memory>
#include "USBAction.h"

namespace tfw::hal::usb {
    class Connection {
    public:
        virtual ~Connection() = default;

        virtual void setup() = 0;
        virtual void update() = 0;
        virtual bool isConnected() = 0;

        virtual void sendAction(Action& action) = 0;

        // Time to wait between characters typed by type actions
        virtual void setTypingDelayMs(uint32_t delayMs) = 0;

        static std::unique_ptr<Connection> create();
    };
}