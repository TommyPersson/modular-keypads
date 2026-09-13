#include <tinyNeoPixel.h>

#include "ic/L74165.h"

#define PIN PIN_PC5
#define NUMPIXELS 6

using namespace smc::ic::L74165;

namespace {
    auto pixels = tinyNeoPixel(NUMPIXELS, PIN_PC5, NEO_GRB + NEO_KHZ800);

    auto l74165 = L74165({
        .pinQ = PIN_PB3,
        .pinCLK = PIN_PB6,
        .pinCE = PIN_PB4,
        .pinLD = PIN_PB5,
    });

    uint8_t smcType = 0;
}


void setup() {
    Serial.swap(1);
    Serial.begin(9600);
    pinMode(PIN_PC5, OUTPUT);

    delayMicroseconds(100);
    digitalWrite(PIN_PC5, LOW);
    delayMicroseconds(100);

    pixels.begin();
    pixels.setPixelColor(5, tinyNeoPixel::Color(1, 0, 0));
    pixels.setPixelColor(4, tinyNeoPixel::Color(0, 1, 0));
    pixels.setPixelColor(3, tinyNeoPixel::Color(0, 0, 1));
    pixels.setPixelColor(2, tinyNeoPixel::Color(1, 1, 0));
    pixels.setPixelColor(1, tinyNeoPixel::Color(0, 1, 1));
    pixels.setPixelColor(0, tinyNeoPixel::Color(1, 0, 1));
    pixels.show();

    l74165.setup();
    l74165.parallelLoad();
    smcType = l74165.read();

    Serial.printf("Hello World! (smcType = %02x)\r\n", smcType);
}

void loop() {
    delayMicroseconds(50);
}
