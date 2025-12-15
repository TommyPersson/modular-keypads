#include "NoOpRegisterRefresher.h"

NoOpRegisterRefresher::NoOpRegisterRefresher(RegisterManager& registers) :
    RegisterRefresher(registers) {
}

NoOpRegisterRefresher::~NoOpRegisterRefresher() {
}

void NoOpRegisterRefresher::begin() {
}

void NoOpRegisterRefresher::loop() {
}
