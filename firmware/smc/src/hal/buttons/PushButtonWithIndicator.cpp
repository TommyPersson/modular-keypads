#include "PushButtonWithIndicator.h"

using namespace smc::hal::buttons;

PushButtonWithIndicator::PushButtonWithIndicator(
    const uint8_t number,
    const uint8_t pin,
    leds::IndicatorLeds& leds,
    const uint8_t ledIndex
) : button(number, pin),
    leds(leds),
    ledIndex(ledIndex) {
}

void PushButtonWithIndicator::setup() {
    button.setup();
}

void PushButtonWithIndicator::update(const uint32_t deltaUs, events::EventQueue& queue) {
    button.update(deltaUs, queue);

    const uint8_t level = button.isPressed() ? 255 : 0;
    const uint8_t brightness = button.isLongPressed() ? LED_BRIGHTNESS_LONG_PRESSED : LED_BRIGHTNESS_PRESSED;

    leds.setColor(ledIndex, level, level, level, brightness);
}
