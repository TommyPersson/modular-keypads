#pragma once

#include <tfw/utils/commands.h>

class PingCommandHandler final : public tfw::utils::commands::CommandHandler {
public:
    explicit PingCommandHandler();
    ~PingCommandHandler() override;

    tfw::utils::void_result execute(
        const std::span<const std::string_view>& args,
        tfw::utils::commands::CommandResponseWriter& responseWriter,
        tfw::utils::allocations::Arena& arena
    ) override;
};
