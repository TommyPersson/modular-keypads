#pragma once

#include <tfw/utils/commands.h>
#include "mkp/components/macros/MacroStorage.h"

namespace mkp::firmwares::master::commands {
    class ListStoredMacros : public tfw::utils::commands::CommandHandler {
    public:
        explicit ListStoredMacros(mkp::components::macros::MacroStorage& macroStorage);
        ~ListStoredMacros() override;

        tfw::utils::void_result execute(
            const std::span<const std::string_view>& args,
            tfw::utils::commands::CommandResponseWriter& responseWriter,
            tfw::utils::allocations::Arena& arena
        ) override;

    private:
        mkp::components::macros::MacroStorage& macroStorage;
    };
}