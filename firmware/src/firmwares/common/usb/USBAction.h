#pragma once

#include <memory>

namespace usb {
    struct Action {
        virtual ~Action() = default;

        static std::shared_ptr<Action> keyPress(uint8_t keyCode);
    };

    struct ConsumerControlAction final : Action {
        ~ConsumerControlAction() override = default;
    };


    struct KeyPressData {
        uint8_t keyCode;
    };

    struct KeyPressAction final : Action {
        ~KeyPressAction() override = default;

        explicit KeyPressAction(const KeyPressData data) : data(data) {}

        KeyPressData data;
    };
}
