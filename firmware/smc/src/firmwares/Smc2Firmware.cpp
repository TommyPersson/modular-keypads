#include "Smc2Firmware.h"

#include <tinyNeoPixel.h>
#include <Arduino.h>

namespace {
    auto pixels = tinyNeoPixel(6, PIN_PC5, NEO_GRB + NEO_KHZ800);
}

using namespace smc::firmwares;

Smc2Firmware::Smc2Firmware() {
}

Smc2Firmware::~Smc2Firmware() {
}

void Smc2Firmware::setup() {
    Firmware::setup();

    pixels.begin();
    pixels.setBrightness(50);
    pixels.setPixelColor(5, tinyNeoPixel::Color(255, 0, 0));
    pixels.setPixelColor(4, tinyNeoPixel::Color(0, 255, 0));
    pixels.setPixelColor(3, tinyNeoPixel::Color(0, 0, 255));
    pixels.setPixelColor(2, tinyNeoPixel::Color(255, 255, 0));
    pixels.setPixelColor(1, tinyNeoPixel::Color(0, 255, 255));
    pixels.setPixelColor(0, tinyNeoPixel::Color(255, 0, 255));
    pixels.show();

    for (auto& button : buttons) {
        button.setup();
    }

    Serial.printf("SMC2 firmware setup complete!\r\n");
}

void Smc2Firmware::update(const uint32_t deltaUs, smc::events::EventQueue& queue) {
    Firmware::update(deltaUs, queue);

    for (auto& button : buttons) {
        button.update(deltaUs, queue);

        auto ledIndex = buttonLedMap[button.number - 1];
        if (button.isPressed()) {
            pixels.setPixelColor(ledIndex, tinyNeoPixel::Color(255, 255, 255));
        } else {
            pixels.setPixelColor(ledIndex, tinyNeoPixel::Color(0, 0, 0));
        }
    }

    pixels.show();
}
