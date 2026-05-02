#include "L74165.h"

#include <tfw/hal/time/Time.h>

tfw::ic::L74165::L74165(Config config) : config(std::move(config)) {
}

void tfw::ic::L74165::setup() {
    config.pinCE->init();
    config.pinCLK->init();
    config.pinLD->init();
    config.pinQ->init();

    config.pinLD->setHigh();
    config.pinCE->setHigh();
    config.pinCLK->setHigh();
}

void tfw::ic::L74165::parallelLoad() {
    config.pinLD->setLow();
    tfw::hal::time::delayUs(1);
    config.pinLD->setHigh();
    tfw::hal::time::delayUs(1);
}

uint8_t tfw::ic::L74165::read() {
    config.pinCLK->setLow();
    config.pinCE->setLow();

    tfw::hal::time::delayUs(1);

    uint8_t result = 0;

    for (int i = 7; i >= 0; i--) {
        const uint8_t data = config.pinQ->read();
        result |= data << i;

        config.pinCLK->setHigh();
        tfw::hal::time::delayUs(1);
        config.pinCLK->setLow();
        tfw::hal::time::delayUs(1);
    }

    config.pinCE->setHigh();

    return result;
}
