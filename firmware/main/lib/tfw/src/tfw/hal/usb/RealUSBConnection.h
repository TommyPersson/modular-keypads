#pragma once

#include "tfw/hal/time/Clock.h"

#include "ActionSender.h"
#include "RealHostComputer.h"
#include "USBConnection.h"

namespace tfw::hal::usb {
    // Connection through the USB stack of the ESP32 Arduino core, sending actions with an ActionSender
    class RealConnection final : public Connection {
    public:
        explicit RealConnection(time::Clock& clock);

        void setup() override;
        void update() override;
        bool isConnected() override;
        void sendAction(Action& action) override;
        void setTypingDelayMs(uint32_t delayMs) override;
        void setModifierHoldMs(uint32_t holdMs) override;
    private:
        time::Clock& clock;
        RealHostComputer hostComputer;
        ActionSender actionSender;
    };
}
