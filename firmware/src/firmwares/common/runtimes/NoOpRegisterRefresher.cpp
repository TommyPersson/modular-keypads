#include "NoOpRegisterRefresher.h"

NoOpRegisterRefresher::NoOpRegisterRefresher(utils::registers::RegisterManager& registers) :
    RegisterRefresher(registers) {
}

NoOpRegisterRefresher::~NoOpRegisterRefresher() {
}

void NoOpRegisterRefresher::begin() {
}

void NoOpRegisterRefresher::loop() {
}
