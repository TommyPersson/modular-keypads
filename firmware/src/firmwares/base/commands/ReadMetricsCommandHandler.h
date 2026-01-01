#pragma once

#include <Registers/Registers.h>

#include "firmwares/common/commands/CommandHandler.h"

class ReadMetricsCommandHandler final : public CommandHandler {
public:
    explicit ReadMetricsCommandHandler();
    ~ReadMetricsCommandHandler() override;

    utils::void_result execute(
        const std::span<const std::string_view>& args,
        CommandResponseWriter& responseWriter,
        Arena& arena
    ) override;

private:
};
