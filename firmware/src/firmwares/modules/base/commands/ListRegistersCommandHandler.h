#pragma once

#include <Registers/Registers.h>

#include "firmwares/common/commands/CommandHandler.h"

class ListRegistersCommandHandler final : public CommandHandler {
public:
    explicit ListRegistersCommandHandler(const RegisterManager& registers, Logger& logger);
    ~ListRegistersCommandHandler() override;

    void execute(
        const std::span<const std::string_view>& args,
        CommandResponseWriter& responseWriter,
        Arena& arena
    ) override;

private:
    const RegisterManager& registers;
};
