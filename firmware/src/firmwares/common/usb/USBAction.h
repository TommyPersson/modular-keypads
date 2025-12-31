#pragma once

#include <memory>
#include <vector>

namespace usb {
    struct Action {
        virtual ~Action() = default;

        static std::shared_ptr<Action> keyPress(const std::vector<uint8_t>& keyCodes);
        static std::shared_ptr<Action> consumerControl(uint16_t usageId);
    };

    struct ConsumerControlAction final : Action {
        explicit ConsumerControlAction(uint16_t usageId) : usageId(usageId) {
        }

        ~ConsumerControlAction() override = default;

        uint16_t usageId;
    };


    struct KeyPressData {
        std::vector<uint8_t> keyCodes;
    };

    struct KeyPressAction final : Action {
        ~KeyPressAction() override = default;

        explicit KeyPressAction(const KeyPressData& data) : data(data) {
        }

        KeyPressData data;
    };
}
