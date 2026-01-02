#pragma once

#include <optional>
#include <Registers/Registers.h>

#include "utils/commands/CommandHandler.h"

class ListRegistersCommandHandler final : public utils::commands::CommandHandler {
public:
    explicit ListRegistersCommandHandler(const std::optional<RegisterManager*>& registers);
    ~ListRegistersCommandHandler() override;

    utils::void_result execute(
        const std::span<const std::string_view>& args,
        utils::commands::CommandResponseWriter& responseWriter,
        utils::allocations::Arena& arena
    ) override;

private:
    const std::optional<RegisterManager*>& registers;
};
