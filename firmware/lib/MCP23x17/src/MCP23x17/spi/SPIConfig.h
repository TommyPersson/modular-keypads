#pragma once

#include "Pins/OutputPin.h"
#include "Pins/InputPin.h"


struct SPIConfig {
    uint8_t spiBus;
    OutputPin pinSCK;
    OutputPin pinMOSI;
    std::shared_ptr<InputPin> pinMISO;
    OutputPin pinCS;
};

