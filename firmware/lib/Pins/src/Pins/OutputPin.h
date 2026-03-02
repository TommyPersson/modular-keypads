#pragma once
#include <memory>

namespace utils::pins {
    class OutputPin {
    public:
        explicit OutputPin(std::uint8_t pinNumber);

        void init() const;

        void set(std::uint8_t state) const;
        void setHigh() const;
        void setLow() const;

        const std::uint8_t pinNumber;

        static std::unique_ptr<OutputPin> physical(std::uint8_t pinNumber);
    };
}
