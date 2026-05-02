#pragma once

#include "../gpio/OutputPin.h"
#include "../gpio/InputPin.h"

namespace tfw::hal::spi {
struct SPIConfig {
    uint8_t spiBus;
    tfw::utils::gpio::OutputPin pinSCK;
    tfw::utils::gpio::OutputPin pinMOSI;
    std::shared_ptr<tfw::utils::gpio::InputPin> pinMISO;
    tfw::utils::gpio::OutputPin pinCS;
};
}