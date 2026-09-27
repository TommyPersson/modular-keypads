#include <tinyNeoPixel.h>

#include "firmwares/Firmware.h"
#include "firmwares/FirmwareFactory.h"

namespace {
    auto firmwareFactory = smc::firmwares::FirmwareFactory(
        {
            .pinQ = PIN_PB3,
            .pinCLK = PIN_PB6,
            .pinCE = PIN_PB4,
            .pinLD = PIN_PB5,
        }
    );

    smc::firmwares::Firmware* firmware = nullptr;
    smc::events::EventQueue eventQueue;
    smc::events::EventQueue spiEventQueue;

    uint32_t prevUs = 0;
}

void setup() {
    Serial.swap(1);
    Serial.begin(9600);
    pinMode(PIN_PC5, OUTPUT);

    delayMicroseconds(100);
    digitalWrite(PIN_PC5, LOW);
    delayMicroseconds(100);

    firmware = firmwareFactory.create();
    firmware->setup();
}

void loop() {
    delayMicroseconds(500);

    const auto currentUs = micros();
    const auto deltaUs = currentUs - prevUs;
    prevUs = currentUs;

    firmware->update(deltaUs, eventQueue);

    while (const auto* event = eventQueue.dequeue()) {
        spiEventQueue.enqueue(*event);

        Serial.printf("%i %i\n\r", event->type, event->data);
    }
}
