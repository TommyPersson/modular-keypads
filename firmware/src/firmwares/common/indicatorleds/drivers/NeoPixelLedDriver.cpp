#include "NeoPixelLedDriver.h"

NeoPixelLedDriver::NeoPixelLedDriver(uint16_t numberOfPixels, int16_t pin, neoPixelType type) :
    neoPixel(numberOfPixels, pin, type) {
}

NeoPixelLedDriver::~NeoPixelLedDriver() = default;

void NeoPixelLedDriver::begin() {
    neoPixel.begin();
    neoPixel.setBrightness(255);
}

void NeoPixelLedDriver::loop() {
}

void NeoPixelLedDriver::show() {
    if (isDirty) {
        neoPixel.show();
        isDirty = false;
    }
}

void NeoPixelLedDriver::setPixelColor(uint8_t uint8, uint32_t color) {
    neoPixel.setPixelColor(uint8, color);
    isDirty = true;
}

uint32_t NeoPixelLedDriver::makeColor(uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
    return Adafruit_NeoPixel::Color(r, g, b, w);
}
