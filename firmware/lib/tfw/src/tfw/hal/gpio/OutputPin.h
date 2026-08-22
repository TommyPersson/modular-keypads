#pragma once
#include <memory>
#include <cstdint>

namespace tfw::hal::gpio {
    class OutputPin {
    public:
        explicit OutputPin(std::uint8_t pinNumber, std::uint8_t modeFlags = 0);
        virtual ~OutputPin() = default;

        virtual void init() const = 0;

        virtual void set(std::uint8_t state) const = 0;
        virtual void setHigh() const = 0;
        virtual void setLow() const = 0;

        const std::uint8_t pinNumber;
        const std::uint8_t modeFlags;

#ifdef ARDUINO
        static std::unique_ptr<OutputPin> physical(std::uint8_t pinNumber, std::uint8_t modeFlags = 0);
#endif
    };
}
