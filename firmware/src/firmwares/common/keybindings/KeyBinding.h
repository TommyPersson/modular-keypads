#pragma once

#include <memory>

#include "firmwares/common/monitors/RotationalEncoderMonitor.h"

namespace common::keybindings {
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
        explicit PushButtonTrigger(const uint64_t deviceId, const uint8_t number)
            : Trigger(deviceId, PUSH_BUTTON),
              number(number) {}

        ~PushButtonTrigger() override = default;

        uint8_t number;

        bool operator==(const Trigger& other) const override {
            const auto otherPBT = dynamic_cast<const PushButtonTrigger*>(&other);
            if (otherPBT == nullptr) {
                return false;
            }

            return Trigger::operator==(other) && otherPBT->number == number;
        }
    };

    struct RotaryEncoderTrigger final : Trigger {
        explicit RotaryEncoderTrigger(const uint64_t deviceId, const uint8_t number, const RotationalEncoderDirection direction)
            : Trigger(deviceId, ROTARY_ENCODER),
              number(number),
              direction(direction) {}

        ~RotaryEncoderTrigger() override = default;

        uint8_t number;
        RotationalEncoderDirection direction;

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
