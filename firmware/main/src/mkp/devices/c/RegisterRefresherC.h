#pragma once

#include <tfw/ic/MCP23x17.h>
#include <tfw/utils/registers.h>
#include <tfw/hal/time.h>

#include "../common/RegisterRefresher.h"

namespace mkp::devices::c {
    class RegisterRefresherC final : public common::RegisterRefresher {
    public:
        explicit RegisterRefresherC(tfw::utils::registers::RegisterManager& registers, tfw::hal::time::Clock& clock);
        ~RegisterRefresherC() override;

        void setup() override;
        void loop() override;
    };
}
