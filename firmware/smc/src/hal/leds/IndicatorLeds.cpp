#include "IndicatorLeds.h"

#include <Arduino.h>

using namespace smc::hal::leds;

namespace {
    constexpr uint16_t RESET_US = 200;

    uint8_t scale(const uint8_t channel, const uint8_t brightness) {
        return (uint16_t)channel * (brightness + 1) >> 8;
    }
}

IndicatorLeds::IndicatorLeds(const uint8_t count, const uint8_t pin)
    : count(count),
      pixels(count, pin, NEO_GRB + NEO_KHZ800),
      colors(new Color[count]()) {
}

IndicatorLeds::~IndicatorLeds() {
    delete[] colors;
}

void IndicatorLeds::setup() {
    pixels.begin();
    delayMicroseconds(RESET_US);

    dirty = true;
}

void IndicatorLeds::update(uint32_t deltaUs, events::EventQueue& queue) {
    if (dirty && count > 0) {
        pixels.show();
    }

    dirty = false;
}

void IndicatorLeds::setColor(const uint8_t index, const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t brightness) {
    if (index >= count) {
        return;
    }

    const Color color = { scale(r, brightness), scale(g, brightness), scale(b, brightness) };
    if (colors[index] == color) {
        return;
    }

    colors[index] = color;
    pixels.setPixelColor(index, color.r, color.g, color.b);
    dirty = true;
}
