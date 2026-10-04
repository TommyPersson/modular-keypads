#pragma once

#ifdef ARDUINO

#include <memory>

#include "../gpio/OutputPin.h"
#include "../gpio/InputPin.h"

namespace tfw::hal::spi {
struct SPIConfig {
    uint8_t spiBus;
    std::shared_ptr<gpio::OutputPin> pinSCK;
    std::shared_ptr<gpio::OutputPin> pinMOSI;
    std::shared_ptr<gpio::InputPin> pinMISO;
    std::shared_ptr<gpio::OutputPin> pinCS;
};
}

#endif // ARDUINO