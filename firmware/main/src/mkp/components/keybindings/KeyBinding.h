#pragma once

#include <memory>
#include <tfw/hal/buttons/Button.h>
#include <tfw/hal/encoders/RotaryEncoder.h>

namespace mkp::components::keybindings {
    enum TriggerType {
        PUSH_BUTTON = 0x01,
        ROTARY_ENCODER = 0x02,
    };

    struct Trigger {
        explicit Trigger(const uint64_t deviceId, const TriggerType type) : deviceId(deviceId), type(type) {}
        virtual ~Trigger() = default;

        uint64_t deviceId;
        TriggerType type;

        virtual bool operator==(const Trigger& other) const {
            return other.deviceId == deviceId && other.type == type;
        };
    };

    struct PushButtonTrigger final : Trigger {
        // The state is PRESSED or LONG_PRESSED, so a button can have a binding for each
        explicit PushButtonTrigger(
            const uint64_t deviceId,
            const uint8_t number,
            const tfw::hal::buttons::ButtonState state = tfw::hal::buttons::ButtonState::PRESSED
        ) : Trigger(deviceId, PUSH_BUTTON),
            number(number),
            state(state) {}

        ~PushButtonTrigger() override = default;

        uint8_t number;
        tfw::hal::buttons::ButtonState state;

        bool operator==(const Trigger& other) const override {
            const auto otherPBT = dynamic_cast<const PushButtonTrigger*>(&other);
            if (otherPBT == nullptr) {
                return false;
            }

            return Trigger::operator==(other) && otherPBT->number == number && otherPBT->state == state;
        }
    };

    struct RotaryEncoderTrigger final : Trigger {
        explicit RotaryEncoderTrigger(const uint64_t deviceId, const uint8_t number, const tfw::hal::encoders::RotaryEncoderDirection direction)
            : Trigger(deviceId, ROTARY_ENCODER),
              number(number),
              direction(direction) {}

        ~RotaryEncoderTrigger() override = default;

        uint8_t number;
        tfw::hal::encoders::RotaryEncoderDirection direction;

        bool operator==(const Trigger& other) const override {
            const auto otherRET = dynamic_cast<const RotaryEncoderTrigger*>(&other);
            if (otherRET == nullptr) {
                return false;
            }

            return Trigger::operator==(other) && otherRET->number == number && otherRET->direction == direction;
        }
    };

    struct KeyBinding {
        std::shared_ptr<Trigger> trigger;
        uint16_t macroId;
    };
}
