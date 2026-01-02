#pragma once

#include <optional>
#include <Registers/Registers.h>

#include "utils/commands/CommandHandler.h"

class ListRegisterValuesCommandHandler final : public utils::commands::CommandHandler {
public:
    explicit ListRegisterValuesCommandHandler(const std::optional<utils::registers::RegisterManager*>& registers);
    ~ListRegisterValuesCommandHandler() override;

    utils::void_result execute(
        const std::span<const std::string_view>& args,
        utils::commands::CommandResponseWriter& responseWriter,
        utils::allocations::Arena& arena
    ) override;

private:
    const std::optional<utils::registers::RegisterManager*>& registers;
};
