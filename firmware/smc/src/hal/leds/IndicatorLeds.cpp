#include "IndicatorLeds.h"

#include <Arduino.h>
#include <string.h>

using namespace smc::hal::leds;

namespace {
    constexpr uint8_t BYTES_PER_PIXEL = 3;  // RGB-type LEDs (NEO_GRB)
    constexpr uint16_t RESET_US = 200;
}

IndicatorLeds::IndicatorLeds(const uint8_t count, const uint8_t pin)
    : pixels(count, pin, NEO_GRB + NEO_KHZ800) {
}

void IndicatorLeds::setup() {
    pixels.begin();
    delayMicroseconds(RESET_US);

    dirty = true;
}

void IndicatorLeds::update(uint32_t deltaUs, events::EventQueue& queue) {
    if (dirty && pixels.numPixels() > 0) {
        pixels.show();
    }

    dirty = false;
}

void IndicatorLeds::setColor(const uint8_t index, const uint8_t r, const uint8_t g, const uint8_t b) {
    if (index >= pixels.numPixels()) {
        return;
    }

    // Compare the stored bytes, as getPixelColor() is only approximate once a brightness is set
    const uint8_t* stored = pixels.getPixels() + index * BYTES_PER_PIXEL;
    uint8_t previous[BYTES_PER_PIXEL];
    memcpy(previous, stored, BYTES_PER_PIXEL);

    pixels.setPixelColor(index, r, g, b);

    if (memcmp(previous, stored, BYTES_PER_PIXEL) != 0) {
        dirty = true;
    }
}

void IndicatorLeds::setBrightness(const uint8_t brightness) {
    pixels.setBrightness(brightness);
    dirty = true;
}
