#pragma once

#include <tfw/utils/commands.h>
#include "mkp/components/macros/MacroStorage.h"

namespace mkp::firmwares::master::commands {
    class SaveMacro : public tfw::utils::commands::CommandHandler {
    public:
        explicit SaveMacro(mkp::components::macros::MacroStorage& macroStorage);
        ~SaveMacro() override;

        tfw::utils::void_result execute(
            const std::span<const std::string_view>& args,
            tfw::utils::commands::CommandResponseWriter& responseWriter,
            tfw::utils::allocations::Arena& arena
        ) override;

    private:
        mkp::components::macros::MacroStorage& macroStorage;
    };
}