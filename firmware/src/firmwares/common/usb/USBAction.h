#pragma once

#include <memory>
#include <vector>

namespace usb {
    struct Action {
        virtual ~Action() = default;

        static std::shared_ptr<Action> keyPress(const std::vector<uint8_t>& keyCodes);
    };

    struct ConsumerControlAction final : Action {
        ~ConsumerControlAction() override = default;
    };


    struct KeyPressData {
        std::vector<uint8_t> keyCodes;
    };

    struct KeyPressAction final : Action {
        ~KeyPressAction() override = default;

        explicit KeyPressAction(const KeyPressData& data) : data(data) {}

        KeyPressData data;
    };
}
