#include "BootButton.h"

#include <Arduino.h>

namespace {
    constexpr uint8_t BOOT_BUTTON_PIN = 0;

    // Shorter presses are taken to be contact bounce
    constexpr uint64_t MINIMUM_PRESS_US = 50'000;
}

using namespace mkp::firmwares::drive;

BootButton::BootButton(const tfw::hal::time::Clock& clock, std::function<void()> onClicked)
    : clock(clock),
      onClicked(std::move(onClicked)) {
}

BootButton::~BootButton() = default;

void BootButton::setup() {
    pin = tfw::hal::gpio::InputPin::physical(BOOT_BUTTON_PIN, INPUT_PULLUP);
    pin->setup();
}

void BootButton::loop() {
    const auto isPressed = pin->read() == LOW;
    const auto now = clock.micros();

    if (isPressed) {
        if (!pressedAtUs) {
            pressedAtUs = now;
        }
        return;
    }

    const auto isClicked = pressedAtUs && now - *pressedAtUs >= MINIMUM_PRESS_US;
    pressedAtUs.reset();

    if (isClicked) {
        onClicked();
    }
}
