#include "Smc3Firmware.h"

#include <Arduino.h>

using namespace smc::firmwares;

Smc3Firmware::Smc3Firmware()
    : Firmware(2) {
}

Smc3Firmware::~Smc3Firmware() = default;

void Smc3Firmware::doSetup() {
    encoder.setup();
    button.setup();

    Serial.printf("SMC3 firmware setup complete!\r\n");
}

void Smc3Firmware::doUpdate(const uint32_t deltaUs, smc::events::EventQueue& queue) {
    encoder.update(deltaUs, queue);
    button.update(deltaUs, queue);

    // Only the latest direction is lit, so reversing switches sides right away
    const auto rotation = encoder.getRotation();
    if (rotation == events::types::ROTARY_ENCODER_ROTATED_CLOCKWISE) {
        clockwiseLedRemainingUs = ROTATION_LED_ON_US;
        counterClockwiseLedRemainingUs = 0;
    } else if (rotation == events::types::ROTARY_ENCODER_ROTATED_COUNTER_CLOCKWISE) {
        counterClockwiseLedRemainingUs = ROTATION_LED_ON_US;
        clockwiseLedRemainingUs = 0;
    }

    updateLed(CLOCKWISE_LED_INDEX, clockwiseLedRemainingUs, deltaUs);
    updateLed(COUNTER_CLOCKWISE_LED_INDEX, counterClockwiseLedRemainingUs, deltaUs);
}

int8_t Smc3Firmware::ledIndexForButton(const uint8_t buttonNumber) {
    // The button lights both LEDs, but identification flashes a single one
    return buttonNumber == button.number ? CLOCKWISE_LED_INDEX : -1;
}

void Smc3Firmware::updateLed(const uint8_t ledIndex, uint32_t& remainingUs, const uint32_t deltaUs) {
    // Set once per update, as alternating colors would refresh the LEDs every loop
    const bool lit = button.isPressed() || remainingUs > 0;
    const uint8_t level = lit ? 255 : 0;
    const uint8_t brightness = button.isLongPressed() ? LED_BRIGHTNESS_LONG_PRESSED : LED_BRIGHTNESS;
    leds.setColor(ledIndex, level, level, level, brightness);

    remainingUs = remainingUs > deltaUs ? remainingUs - deltaUs : 0;
}
