#pragma once

#include "utils/commands/CommandHandler.h"

class PingCommandHandler final : public utils::commands::CommandHandler {
public:
    explicit PingCommandHandler();
    ~PingCommandHandler() override;

    utils::void_result execute(
        const std::span<const std::string_view>& args,
        utils::commands::CommandResponseWriter& responseWriter,
        utils::allocations::Arena& arena
    ) override;
};
