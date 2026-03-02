#include "L74165.h"

#include <esp32-hal.h>

chips::L74165::L74165::L74165(Config config) : config(std::move(config)) {
}

void chips::L74165::L74165::setup() {
    config.pinCE->init();
    config.pinCLK->init();
    config.pinLD->init();
    config.pinQ->init();

    config.pinLD->setHigh();
    config.pinCE->setHigh();
    config.pinCLK->setHigh();
}

void chips::L74165::L74165::parallelLoad() {
    config.pinLD->setLow();
    delayMicroseconds(1);
    config.pinLD->setHigh();
    delayMicroseconds(1);
}

uint8_t chips::L74165::L74165::read() {
    config.pinCLK->setLow();
    config.pinCE->setLow();

    delayMicroseconds(1);

    uint8_t result = 0;

    for (int i = 7; i >= 0; i--) {
        const uint8_t data = config.pinQ->read();
        result |= data << i;

        config.pinCLK->setHigh();
        delayMicroseconds(1);
        config.pinCLK->setLow();
        delayMicroseconds(1);
    }

    config.pinCE->setHigh();

    return result;
}
