#include "OutputPin.h"

namespace tfw::hal::gpio {
    OutputPin::OutputPin(const std::uint8_t pinNumber, const std::uint8_t modeFlags)
        : pinNumber(pinNumber), modeFlags(modeFlags) {
    }
}

#ifdef ARDUINO

#include <Arduino.h>

namespace tfw::hal::gpio {
    namespace {
        class PhysicalOutputPin : public OutputPin {
        public:
            explicit PhysicalOutputPin(std::uint8_t pinNumber, std::uint8_t modeFlags)
                : OutputPin(pinNumber, modeFlags) {}

            void init() const override {
                pinMode(pinNumber, OUTPUT | modeFlags);
            }

            void set(std::uint8_t state) const override {
                digitalWrite(pinNumber, state);
            }

            void setHigh() const override {
                digitalWrite(pinNumber, HIGH);
            }

            void setLow() const override {
                digitalWrite(pinNumber, LOW);
            }
        };
    }

    std::unique_ptr<OutputPin> OutputPin::physical(std::uint8_t pinNumber, std::uint8_t modeFlags) {
        return std::make_unique<PhysicalOutputPin>(pinNumber, modeFlags);
    }
}

#endif // ARDUINO
