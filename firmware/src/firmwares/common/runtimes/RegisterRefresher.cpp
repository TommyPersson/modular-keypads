#include "RegisterRefresher.h"

RegisterRefresher::RegisterRefresher(utils::registers::RegisterManager& registers) :
    registers(registers) {
}

RegisterRefresher::~RegisterRefresher() = default;
