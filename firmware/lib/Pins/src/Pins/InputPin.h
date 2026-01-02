#pragma once

#include <cstdint>
#include <memory>

namespace utils::pins {
    class InputPin {
    public:
        explicit InputPin(std::uint8_t pinNumber);
        virtual ~InputPin() = default;

        virtual void init() const = 0;

        virtual std::uint8_t read() const = 0;

        static std::unique_ptr<InputPin> physical(std::uint8_t pinNumber);
        static std::unique_ptr<InputPin> physical(std::uint8_t pinNumber, std::uint8_t modeFlags);

        const std::uint8_t pinNumber;
    };
}