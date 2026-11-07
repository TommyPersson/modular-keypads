#pragma once

#include "tfw/hal/gpio/InputPin.h"
#include "tfw/hal/time/Clock.h"

#include "ActionSender.h"
#include "RealHostComputer.h"
#include "USBConnection.h"

namespace tfw::hal::usb {
    class RealConnection final : public Connection {
    public:
        RealConnection(time::Clock& clock, std::unique_ptr<gpio::InputPin> vbusPin);

        void setup() override;
        void update() override;
        bool isStarted() override;
        bool isConnected() override;
        void sendAction(Action& action) override;
        bool isBusy() override;
        void setTypingDelayMs(uint32_t delayMs) override;
        void setModifierHoldMs(uint32_t holdMs) override;
    private:
        bool isVbusPresent() const;

        time::Clock& clock;
        std::unique_ptr<gpio::InputPin> vbusPin;
        bool started = false;
        RealHostComputer hostComputer;
        ActionSender actionSender;
    };
}
