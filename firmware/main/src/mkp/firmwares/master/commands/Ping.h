#pragma once

#include <tfw/utils/commands.h>
namespace mkp::firmwares::master::commands {
    class Ping final : public tfw::utils::commands::CommandHandler {
    public:
        explicit Ping();
        ~Ping() override;

        tfw::utils::void_result execute(
            const std::span<const std::string_view>& args,
            tfw::utils::commands::CommandResponseWriter& responseWriter,
            tfw::utils::allocations::Arena& arena
        ) override;
    };
}