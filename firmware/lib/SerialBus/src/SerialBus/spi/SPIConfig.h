#pragma once

#include "Pins/OutputPin.h"
#include "Pins/InputPin.h"

namespace tfw::utils::serialbus {
struct SPIConfig {
    uint8_t spiBus;
    tfw::utils::pins::OutputPin pinSCK;
    tfw::utils::pins::OutputPin pinMOSI;
    std::shared_ptr<tfw::utils::pins::InputPin> pinMISO;
    tfw::utils::pins::OutputPin pinCS;
};
}