#pragma once
#include <stdint.h>

namespace smc::ic::L74165 {

    struct Config {
        uint8_t pinQ;
        uint8_t pinCLK;
        uint8_t pinCE;
        uint8_t pinLD;
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

