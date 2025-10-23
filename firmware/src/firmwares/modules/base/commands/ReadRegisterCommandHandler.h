#pragma once

#include <Registers/Registers.h>

#include "firmwares/common/commands/CommandHandler.h"

class ReadRegisterCommandHandler final : public CommandHandler {
public:
    explicit ReadRegisterCommandHandler(const RegisterManager& registers, Logger& logger);
    ~ReadRegisterCommandHandler() override;

    void execute(
        const std::span<const std::string_view>& args,
        CommandResponseWriter& responseWriter,
        Arena& arena
    ) override;

private:
    const RegisterManager& registers;
};
