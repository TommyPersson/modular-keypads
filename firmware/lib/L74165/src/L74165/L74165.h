#pragma once

#include <Pins/Pins.h>

namespace chips::L74165
{
    struct Config {
        std::unique_ptr<tfw::utils::pins::InputPin> pinQ;
        std::unique_ptr<tfw::utils::pins::OutputPin> pinCLK;
        std::unique_ptr<tfw::utils::pins::OutputPin> pinCE;
        std::unique_ptr<tfw::utils::pins::OutputPin> pinLD;
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
