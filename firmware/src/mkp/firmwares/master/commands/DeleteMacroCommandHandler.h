#pragma once

#include <tfw/utils/commands.h>
#include "firmwares/common/macros/MacroStorage.h"

class DeleteMacroCommandHandler : public tfw::utils::commands::CommandHandler {
public:
    explicit DeleteMacroCommandHandler(common::macros::MacroStorage& macroStorage);
    ~DeleteMacroCommandHandler() override;

    tfw::utils::void_result execute(
        const std::span<const std::string_view>& args,
        tfw::utils::commands::CommandResponseWriter& responseWriter,
        tfw::utils::allocations::Arena& arena
    ) override;

private:
    common::macros::MacroStorage& macroStorage;
};
