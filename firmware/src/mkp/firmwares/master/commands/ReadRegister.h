#pragma once

#include <optional>
#include <tfw/utils/registers.h>

#include <tfw/utils/commands.h>

namespace mkp::firmwares::master::commands {
    class ReadRegister final : public tfw::utils::commands::CommandHandler {
    public:
        explicit ReadRegister(const std::optional<tfw::utils::registers::RegisterManager*>& registers);
        ~ReadRegister() override;

        tfw::utils::void_result execute(
            const std::span<const std::string_view>& args,
            tfw::utils::commands::CommandResponseWriter& responseWriter,
            tfw::utils::allocations::Arena& arena
        ) override;

    private:
        const std::optional<tfw::utils::registers::RegisterManager*>& registers;
    };
}