#include "DeviceId.h"

#include <avr/io.h>

using namespace smc::hal::identity;

namespace {
    constexpr uint8_t SERIAL_NUMBER_LENGTH = 10;

    constexpr uint64_t FNV_OFFSET_BASIS = 0xCBF29CE484222325ULL;
    constexpr uint64_t FNV_PRIME = 0x100000001B3ULL;

    uint64_t hashSerialNumber() {
        const volatile uint8_t* serialNumber = &SIGROW.SERNUM0;

        uint64_t hash = FNV_OFFSET_BASIS;
        for (uint8_t i = 0; i < SERIAL_NUMBER_LENGTH; i++) {
            hash ^= serialNumber[i];
            hash *= FNV_PRIME;
        }
        return hash;
    }
}

void smc::hal::identity::computeDeviceId(uint8_t (&id)[DEVICE_ID_LENGTH]) {
    uint64_t hash = hashSerialNumber();
    for (uint8_t i = 0; i < DEVICE_ID_LENGTH; i++) {
        id[i] = static_cast<uint8_t>(hash);
        hash >>= 8;
    }
}
