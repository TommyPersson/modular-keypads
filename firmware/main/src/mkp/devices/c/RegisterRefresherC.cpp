#include "RegisterRefresherC.h"

#include "LocalDeviceC.h"

using namespace mkp::devices::c;

RegisterRefresherC::RegisterRefresherC(tfw::utils::registers::RegisterManager& registers, tfw::hal::time::Clock& clock) :
    RegisterRefresher(registers) {
}

RegisterRefresherC::~RegisterRefresherC() = default;

void RegisterRefresherC::setup() {
}

void RegisterRefresherC::loop() {
}
