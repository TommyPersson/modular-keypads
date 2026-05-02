#pragma once

#include "../gpio/OutputPin.h"
#include "../gpio/InputPin.h"

namespace tfw::hal::spi {
struct SPIConfig {
    uint8_t spiBus;
    tfw::hal::gpio::OutputPin pinSCK;
    tfw::hal::gpio::OutputPin pinMOSI;
    std::shared_ptr<tfw::hal::gpio::InputPin> pinMISO;
    tfw::hal::gpio::OutputPin pinCS;
};
}