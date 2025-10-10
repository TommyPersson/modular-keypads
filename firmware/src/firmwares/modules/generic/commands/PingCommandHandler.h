#pragma once

#include <Stream.h>

#include "firmwares/common/commands/CommandHandler.h"

class PingCommandHandler final : public CommandHandler {
public:
    explicit PingCommandHandler(Logger& logger);
    ~PingCommandHandler() override;

    std::string execute(const std::span<const std::string_view>& args) override;
};
