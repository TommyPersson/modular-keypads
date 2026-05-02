#pragma once

#include <optional>
#include <Registers/Registers.h>

#include <tfw/utils/commands.h>

class ReadRegisterCommandHandler final : public tfw::utils::commands::CommandHandler {
public:
    explicit ReadRegisterCommandHandler(const std::optional<tfw::utils::registers::RegisterManager*>& registers);
    ~ReadRegisterCommandHandler() override;

    tfw::utils::void_result execute(
        const std::span<const std::string_view>& args,
        tfw::utils::commands::CommandResponseWriter& responseWriter,
        tfw::utils::allocations::Arena& arena
    ) override;

private:
    const std::optional<tfw::utils::registers::RegisterManager*>& registers;
};
