#include <tinyNeoPixel.h>

#include "firmwares/Firmware.h"
#include "firmwares/FirmwareFactory.h"
#include "hal/spi/SpiSlave.h"

namespace {
    auto firmwareFactory = smc::firmwares::FirmwareFactory(
        {
            .pinQ = PIN_PB3,
            .pinCLK = PIN_PB6,
            .pinCE = PIN_PB4,
            .pinLD = PIN_PB5,
        }
    );

    smc::hal::spi::SpiSlave spiSlave;
    smc::firmwares::Firmware* firmware = nullptr;
    smc::events::EventQueue eventQueue;

    uint32_t prevUs = 0;
}

void setup() {
    Serial.swap(1);
    Serial.begin(9600);

    spiSlave.setup();

    firmware = firmwareFactory.create();
    firmware->setup();
}

void loop() {
    delayMicroseconds(500);

    const auto currentUs = micros();
    const auto deltaUs = currentUs - prevUs;
    prevUs = currentUs;

    spiSlave.update(deltaUs, eventQueue);
    firmware->update(deltaUs, eventQueue);

    while (const auto* event = eventQueue.dequeue()) {
        spiSlave.enqueueEvent(*event);

        Serial.printf("%i %i\n\r", event->type, event->data);
    }
}
