#pragma once

#include <tfw/ic/MCP23x17.h>
#include <tfw/utils/registers.h>
#include <tfw/hal/time/Clock.h>

#include "../common/RegisterRefresher.h"

namespace mkp::devices::a {
    class RegisterRefresherA final : public common::RegisterRefresher {
    public:
        explicit RegisterRefresherA(tfw::utils::registers::RegisterManager& registers, tfw::hal::time::Clock& clock);
        ~RegisterRefresherA() override;

        void setup() override;
        void loop() override;

    private:
        std::unique_ptr<tfw::ic::MCP23x17> mcp23x17;
    };
}
