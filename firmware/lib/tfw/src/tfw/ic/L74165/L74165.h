#pragma once

#include <tfw/hal/gpio.h>

namespace tfw::ic
{
    struct Config {
        std::unique_ptr<tfw::hal::gpio::InputPin> pinQ;
        std::unique_ptr<tfw::hal::gpio::OutputPin> pinCLK;
        std::unique_ptr<tfw::hal::gpio::OutputPin> pinCE;
        std::unique_ptr<tfw::hal::gpio::OutputPin> pinLD;
    };

    class L74165
    {
    public:
        explicit L74165(Config config);

        void setup();

        void parallelLoad();
        uint8_t read();

    private:
        const Config config;
    };
}
