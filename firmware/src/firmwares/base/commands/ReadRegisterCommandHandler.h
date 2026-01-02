#pragma once

#include <Registers/Registers.h>

#include "utils/commands/CommandHandler.h"

class ReadRegisterCommandHandler final : public CommandHandler {
public:
    explicit ReadRegisterCommandHandler(const std::optional<RegisterManager*>& registers);
    ~ReadRegisterCommandHandler() override;

    utils::void_result execute(
        const std::span<const std::string_view>& args,
        CommandResponseWriter& responseWriter,
        Arena& arena
    ) override;

private:
    const std::optional<RegisterManager*>& registers;
};
