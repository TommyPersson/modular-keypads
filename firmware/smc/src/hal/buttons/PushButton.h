#pragma once

#include "hal/base/Component.h"
#include "utils/events.h"

namespace smc::hal::buttons {
    enum class Event : uint8_t {
        PRESSED = events::types::PUSH_BUTTON_PRESSED,
        RELEASED = events::types::PUSH_BUTTON_RELEASED,
    };

    enum class State : uint8_t {
        PRESSED = 1,
        UNPRESSED = 2,
        UNKNOWN = 3,
    };

    class PushButton : public Component {
    public:
        PushButton(uint8_t number, uint8_t pin);
        ~PushButton() override;

        void setup() override;
        void update(uint32_t deltaUs, events::EventQueue& queue) override;

        bool isPressed() const;

        const uint8_t number;
    private:
        State readState() const;

        uint8_t pin;
        uint32_t timeSinceUpdateUs = 0;
        int32_t timeUntilLongPress = 0;
        bool hasEmittedLongPress = false;
        State state = State::UNKNOWN;
    };

}

