#include "FirmwareFactory.h"

#include "Smc1Firmware.h"
#include "Smc2Firmware.h"
#include "Smc3Firmware.h"
#include "UnknownFirmware.h"

using namespace smc::firmwares;

FirmwareFactory::FirmwareFactory(const ic::L74165::Config typeSelectorConfig)
    : typeSelector(typeSelectorConfig) {
}

Firmware* FirmwareFactory::create() {
    const auto smcType = readSmcType();

    if (smcType == 1) {
        return new Smc1Firmware();
    }
    if (smcType == 2) {
        return new Smc2Firmware();
    }
    if (smcType == 3) {
        return new Smc3Firmware();
    }

    return new UnknownFirmware(smcType);
}

uint8_t FirmwareFactory::readSmcType() {
    typeSelector.setup();
    typeSelector.parallelLoad();
    return typeSelector.read();
}
