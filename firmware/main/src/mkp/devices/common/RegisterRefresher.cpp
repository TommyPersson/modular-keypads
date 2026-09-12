#include "RegisterRefresher.h"

using namespace mkp::devices::common;

RegisterRefresher::RegisterRefresher(tfw::utils::registers::RegisterManager& registers) :
    registers(registers) {
}

RegisterRefresher::~RegisterRefresher() = default;
