#pragma once

#ifdef ARDUINO

#include <memory>

#include "../gpio/OutputPin.h"
#include "../gpio/InputPin.h"

namespace tfw::hal::spi {
struct SPIConfig {
    uint8_t spiBus;
    std::unique_ptr<tfw::hal::gpio::OutputPin> pinSCK;
    std::unique_ptr<tfw::hal::gpio::OutputPin> pinMOSI;
    std::shared_ptr<tfw::hal::gpio::InputPin> pinMISO;
    std::unique_ptr<tfw::hal::gpio::OutputPin> pinCS;
};
}

#endif // ARDUINO