#pragma once

#include <tfw/utils/commands.h>
#include "firmwares/common/macros/MacroStorage.h"

class SaveMacroCommandHandler : public tfw::utils::commands::CommandHandler {
public:
    explicit SaveMacroCommandHandler(common::macros::MacroStorage& macroStorage);
    ~SaveMacroCommandHandler() override;

    tfw::utils::void_result execute(
        const std::span<const std::string_view>& args,
        tfw::utils::commands::CommandResponseWriter& responseWriter,
        tfw::utils::allocations::Arena& arena
    ) override;

private:
    common::macros::MacroStorage& macroStorage;
};
