#pragma once

#include <tfw/ic/MCP23x17.h>
#include <tfw/utils/registers.h>

#include "../common/RegisterRefresher.h"

namespace mkp::devices::a {
    class RegisterRefresherA final : public common::RegisterRefresher {
    public:
        explicit RegisterRefresherA(tfw::utils::registers::RegisterManager& registers);
        ~RegisterRefresherA() override;

        void setup() override;
        void loop() override;

    private:
        std::unique_ptr<tfw::ic::MCP23x17> mcp23x17;
    };
}
