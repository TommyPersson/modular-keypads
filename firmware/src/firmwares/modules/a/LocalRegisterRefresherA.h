#pragma once

#include <MCP23x17/MCP23x17.h>
#include <Registers/RegisterManager.h>

#include "firmwares/common/runtimes/RegisterRefresher.h"

class LocalRegisterRefresherA final : public RegisterRefresher {
public:
    explicit LocalRegisterRefresherA(utils::registers::RegisterManager& registers);
    ~LocalRegisterRefresherA() override;

    void begin() override;
    void loop() override;

private:
    std::unique_ptr<chips::mcp23x17::MCP23x17> mcp23x17;
};
