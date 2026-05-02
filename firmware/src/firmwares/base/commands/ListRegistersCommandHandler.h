#pragma once

#include <optional>
#include <Registers/Registers.h>

#include <tfw/utils/commands.h>

class ListRegistersCommandHandler final : public tfw::utils::commands::CommandHandler {
public:
    explicit ListRegistersCommandHandler(const std::optional<tfw::utils::registers::RegisterManager*>& registers);
    ~ListRegistersCommandHandler() override;

    tfw::utils::void_result execute(
        const std::span<const std::string_view>& args,
        tfw::utils::commands::CommandResponseWriter& responseWriter,
        tfw::utils::allocations::Arena& arena
    ) override;

private:
    const std::optional<tfw::utils::registers::RegisterManager*>& registers;
};
