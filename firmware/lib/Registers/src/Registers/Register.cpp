#include "Register.h"

#include <string>

Register::Register(const std::string& name) :
    value(0),
    name(name) {
}

Register::~Register() = default;

void Register::write(uint8_t newValue) {
    std::lock_guard guard(lock);

    this->value = newValue;
}

uint8_t Register::read() {
    std::lock_guard guard(lock);

    return this->value;
}
