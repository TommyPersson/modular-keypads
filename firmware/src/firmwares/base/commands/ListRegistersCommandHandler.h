#pragma once

#include <Registers/Registers.h>

#include "utils/commands/CommandHandler.h"

class ListRegistersCommandHandler final : public CommandHandler {
public:
    explicit ListRegistersCommandHandler(const std::optional<RegisterManager*>& registers);
    ~ListRegistersCommandHandler() override;

    utils::void_result execute(
        const std::span<const std::string_view>& args,
        CommandResponseWriter& responseWriter,
        Arena& arena
    ) override;

private:
    const std::optional<RegisterManager*>& registers;
};
