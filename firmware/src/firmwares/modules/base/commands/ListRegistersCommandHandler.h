#pragma once

#include <Registers/Registers.h>

#include "firmwares/common/commands/CommandHandler.h"

class ListRegistersCommandHandler final : public CommandHandler {
public:
    explicit ListRegistersCommandHandler(const RegisterManager& registers, Logger& logger);
    ~ListRegistersCommandHandler() override;

    std::string execute(const std::span<const std::string_view>& args, Arena& arena) override;

private:
    const RegisterManager& registers;
};
