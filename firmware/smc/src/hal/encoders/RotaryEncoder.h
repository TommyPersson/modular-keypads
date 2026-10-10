#pragma once

#include "hal/base/Component.h"
#include "utils/events.h"

namespace smc::hal::encoders {
    class RotaryEncoder : public Component {
    public:
        RotaryEncoder(uint8_t number, uint8_t pinA, uint8_t pinB);
        ~RotaryEncoder() override = default;

        void setup() override;
        void update(uint32_t deltaUs, events::EventQueue& queue) override;

        // The event type sent by the last update(), or NONE
        uint8_t getRotation() const { return rotation; }

        const uint8_t number;

    private:
        uint8_t readState() const;

        uint8_t pinA;
        uint8_t pinB;
        uint8_t previousState = 0;
        // The event type of the rotation in progress, or NONE, sent once the encoder rests in a detent
        uint8_t pendingEventType = events::types::NONE;
        uint8_t rotation = events::types::NONE;
    };
}
