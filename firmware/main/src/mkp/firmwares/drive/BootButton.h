#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <optional>

#include <tfw/hal/gpio/InputPin.h>
#include <tfw/hal/time/Clock.h>

namespace mkp::firmwares::drive {
    /**
     * The boot button of the ESP32 module, on GPIO0, which switches between the normal mode and the drive mode.
     *
     * Clicks are reported once the button is released, so that it isn't held through the restart that follows, as
     * GPIO0 is the strapping pin that makes the chip start the ROM's download mode when it's held while the chip resets.
     */
    class BootButton {
    public:
        BootButton(const tfw::hal::time::Clock& clock, std::function<void()> onClicked);
        ~BootButton();

        void setup();
        void loop();

    private:
        const tfw::hal::time::Clock& clock;
        const std::function<void()> onClicked;

        std::unique_ptr<tfw::hal::gpio::InputPin> pin;
        std::optional<uint64_t> pressedAtUs;
    };
}
