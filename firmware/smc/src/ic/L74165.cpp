#include "L74165.h"

#include <api/Common.h>

using namespace smc::ic::L74165;

L74165::L74165(Config config)
    : config(config) {
}

void L74165::setup() {
    pinMode(config.pinQ, INPUT);
    pinMode(config.pinCLK, OUTPUT);
    pinMode(config.pinCE, OUTPUT);
    pinMode(config.pinLD, OUTPUT);

    digitalWrite(config.pinLD, HIGH);
    digitalWrite(config.pinCE, HIGH);
    digitalWrite(config.pinCLK, HIGH);
}

void L74165::parallelLoad() {
    digitalWrite(config.pinLD, LOW);
    delayMicroseconds(1);
    digitalWrite(config.pinLD, HIGH);
    delayMicroseconds(1);
}

uint8_t L74165::read() {
    digitalWrite(config.pinCLK, LOW);
    digitalWrite(config.pinCE, LOW);

    delayMicroseconds(1);

    uint8_t result = 0;

    for (int i = 7; i >= 0; i--) {
        const uint8_t data = digitalRead(config.pinQ);
        result |= data << i;

        digitalWrite(config.pinCLK, HIGH);
        delayMicroseconds(1);
        digitalWrite(config.pinCLK, LOW);
        delayMicroseconds(1);
    }

    digitalWrite(config.pinCE, HIGH);

    return result;
}
