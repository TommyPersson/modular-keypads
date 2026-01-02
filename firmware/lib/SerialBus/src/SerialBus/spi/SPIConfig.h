#pragma once

#include "Pins/OutputPin.h"
#include "Pins/InputPin.h"

namespace utils::serialbus {
struct SPIConfig {
    uint8_t spiBus;
    utils::pins::OutputPin pinSCK;
    utils::pins::OutputPin pinMOSI;
    std::shared_ptr<utils::pins::InputPin> pinMISO;
    utils::pins::OutputPin pinCS;
};
}