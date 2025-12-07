#pragma once

#include <MCP23x17/MCP23x17.h>
#include <Registers/RegisterManager.h>

#include "firmwares/common/runtimes/RegisterRefresher.h"

class LocalRegisterRefresherM final : public RegisterRefresher {
public:
    explicit LocalRegisterRefresherM(RegisterManager& registers);
    ~LocalRegisterRefresherM() override;

    void begin() override;
    void loop() override;

private:
    std::unique_ptr<MCP23x17> mcp23x17;
    std::unique_ptr<OutputPin> mcpResetPin;
};
