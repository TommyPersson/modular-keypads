#pragma once

#include <Registers/Registers.h>

#include "firmwares/common/commands/CommandHandler.h"

class ListRegisterValuesCommandHandler final : public CommandHandler {
public:
    explicit ListRegisterValuesCommandHandler(const std::optional<RegisterManager*>& registers);
    ~ListRegisterValuesCommandHandler() override;

    utils::void_result execute(
        const std::span<const std::string_view>& args,
        CommandResponseWriter& responseWriter,
        Arena& arena
    ) override;

private:
    const std::optional<RegisterManager*>& registers;
};
