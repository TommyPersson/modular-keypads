#pragma once
#include <memory>

namespace tfw::hal::gpio {
    class OutputPin {
    public:
        explicit OutputPin(std::uint8_t pinNumber, std::uint8_t modeFlags = 0);

        void init() const;

        void set(std::uint8_t state) const;
        void setHigh() const;
        void setLow() const;

        const std::uint8_t pinNumber;
        const std::uint8_t modeFlags;

        static std::unique_ptr<OutputPin> physical(std::uint8_t pinNumber, std::uint8_t modeFlags = 0);
    };
}
