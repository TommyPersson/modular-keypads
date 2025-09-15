#include "Register.h"

#include <string.h>

Register::Register(const char* const name) :
    value(0) {
    this->name = strdup(name);
}

Register::~Register() {
    free(this->name);
}

void Register::write(uint8_t newValue) {
    std::lock_guard guard(lock);

    this->value = newValue;
}

uint8_t Register::read() {
    std::lock_guard guard(lock);

    return this->value;
}
