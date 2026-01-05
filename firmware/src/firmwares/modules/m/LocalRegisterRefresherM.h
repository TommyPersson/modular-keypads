#pragma once

#include <MCP23x17/MCP23x17.h>
#include <Registers/RegisterManager.h>

#include "firmwares/common/runtimes/RegisterRefresher.h"

class LocalRegisterRefresherM final : public RegisterRefresher {
public:
    explicit LocalRegisterRefresherM(utils::registers::RegisterManager& registers);
    ~LocalRegisterRefresherM() override;

    void setup() override;
    void loop() override;

private:
    std::unique_ptr<chips::mcp23x17::MCP23x17> mcp23x17;
    std::unique_ptr<utils::pins::OutputPin> mcpResetPin;
};
