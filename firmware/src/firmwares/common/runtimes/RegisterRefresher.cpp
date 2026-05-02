#include "RegisterRefresher.h"

RegisterRefresher::RegisterRefresher(tfw::utils::registers::RegisterManager& registers) :
    registers(registers) {
}

RegisterRefresher::~RegisterRefresher() = default;
