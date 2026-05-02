#pragma once

#include <tfw/ic/MCP23x17.h>
#include <tfw/utils/registers.h>

#include "firmwares/common/runtimes/RegisterRefresher.h"

class LocalRegisterRefresherM final : public RegisterRefresher {
public:
    explicit LocalRegisterRefresherM(tfw::utils::registers::RegisterManager& registers);
    ~LocalRegisterRefresherM() override;

    void setup() override;
    void loop() override;

private:
    std::unique_ptr<tfw::ic::MCP23x17> mcp23x17;
};
