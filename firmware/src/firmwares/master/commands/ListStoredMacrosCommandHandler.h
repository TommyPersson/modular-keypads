#pragma once

#include <tfw/utils/commands.h>
#include "firmwares/common/macros/MacroStorage.h"

class ListStoredMacrosCommandHandler : public tfw::utils::commands::CommandHandler {
public:
    explicit ListStoredMacrosCommandHandler(common::macros::MacroStorage& macroStorage);
    ~ListStoredMacrosCommandHandler() override;

    tfw::utils::void_result execute(
        const std::span<const std::string_view>& args,
        tfw::utils::commands::CommandResponseWriter& responseWriter,
        tfw::utils::allocations::Arena& arena
    ) override;

private:
    common::macros::MacroStorage& macroStorage;
};
