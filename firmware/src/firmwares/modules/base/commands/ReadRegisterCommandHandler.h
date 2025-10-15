#pragma once

#include <Registers/Registers.h>

#include "firmwares/common/commands/CommandHandler.h"

class ReadRegisterCommandHandler final : public CommandHandler {
public:
    explicit ReadRegisterCommandHandler(const RegisterManager& registers, Logger& logger);
    ~ReadRegisterCommandHandler() override;

    std::string execute(const std::span<const std::string_view>& args, Arena& arena) override;

private:
    const RegisterManager& registers;
};
