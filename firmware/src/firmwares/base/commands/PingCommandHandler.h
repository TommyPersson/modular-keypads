#pragma once

#include <Stream.h>

#include "utils/commands/CommandHandler.h"

class PingCommandHandler final : public CommandHandler {
public:
    explicit PingCommandHandler();
    ~PingCommandHandler() override;

    utils::void_result execute(
        const std::span<const std::string_view>& args,
        CommandResponseWriter& responseWriter,
        Arena& arena
    ) override;
};
