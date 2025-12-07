#pragma once

#include <MCP23x17/MCP23x17.h>
#include <Registers/RegisterManager.h>

#include "firmwares/common/runtimes/RegisterRefresher.h"
#include "RegisterDescriptorsA.h"

class LocalRegisterRefresherA final : public RegisterRefresher {
public:
    explicit LocalRegisterRefresherA(RegisterManager& registers);
    ~LocalRegisterRefresherA() override;

    void begin() override;
    void loop() override;

private:
    std::unique_ptr<MCP23x17> mcp23x17;
};
