#pragma once

#include <tfw/ic/MCP23x17.h>
#include <tfw/utils/registers.h>

#include "firmwares/common/runtimes/RegisterRefresher.h"

class LocalRegisterRefresherA final : public RegisterRefresher {
public:
    explicit LocalRegisterRefresherA(tfw::utils::registers::RegisterManager& registers);
    ~LocalRegisterRefresherA() override;

    void setup() override;
    void loop() override;

private:
    std::unique_ptr<tfw::ic::MCP23x17> mcp23x17;
};
