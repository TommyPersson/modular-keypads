#pragma once

#include <memory>
#include <vector>

namespace utils::usb {
    struct Action {
        virtual ~Action() = default;

        static std::shared_ptr<Action> keyPress(const std::vector<uint8_t>& keyCodes);
        static std::shared_ptr<Action> consumerControl(uint16_t usageId);
        static std::shared_ptr<Action> systemControl(uint8_t code);
    };

    struct ConsumerControlAction final : Action {
        explicit ConsumerControlAction(const uint16_t usageId) : usageId(usageId) {
        }

        ~ConsumerControlAction() override = default;

        const uint16_t usageId;
    };

    struct SystemControlAction final : Action {
        explicit SystemControlAction(const uint8_t code) : code(code) {
        }

        ~SystemControlAction() override = default;

        const uint8_t code;
    };

    struct KeyPressData {
        std::vector<uint8_t> keyCodes;
    };

    struct KeyPressAction final : Action {
        ~KeyPressAction() override = default;

        explicit KeyPressAction(const KeyPressData& data) : data(data) {
        }

        const KeyPressData data;
    };
}
