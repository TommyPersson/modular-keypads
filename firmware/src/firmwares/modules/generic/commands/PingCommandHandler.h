#pragma once

#include <Stream.h>

#include "firmwares/common/commands/CommandHandler.h"

class PingCommandHandler final : public CommandHandler {
public:
    explicit PingCommandHandler();
    ~PingCommandHandler() override;

    String execute() override;
};
