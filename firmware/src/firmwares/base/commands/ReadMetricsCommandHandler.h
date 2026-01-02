#pragma once

#include <Registers/Registers.h>

#include "utils/commands/CommandHandler.h"

class ReadMetricsCommandHandler final : public utils::commands::CommandHandler {
public:
    explicit ReadMetricsCommandHandler();
    ~ReadMetricsCommandHandler() override;

    utils::void_result execute(
        const std::span<const std::string_view>& args,
        utils::commands::CommandResponseWriter& responseWriter,
        utils::allocations::Arena& arena
    ) override;

private:
};
