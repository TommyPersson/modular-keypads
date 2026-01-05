#include "NoOpRegisterRefresher.h"

NoOpRegisterRefresher::NoOpRegisterRefresher(utils::registers::RegisterManager& registers) :
    RegisterRefresher(registers) {
}

NoOpRegisterRefresher::~NoOpRegisterRefresher() {
}

void NoOpRegisterRefresher::setup() {
}

void NoOpRegisterRefresher::loop() {
}
