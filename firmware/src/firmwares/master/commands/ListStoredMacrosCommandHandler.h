#pragma once

#include "utils/commands/CommandHandler.h"
#include "firmwares/common/macros/MacroStorage.h"

class ListStoredMacrosCommandHandler : public utils::commands::CommandHandler {
public:
    explicit ListStoredMacrosCommandHandler(common::macros::MacroStorage& macroStorage);
    ~ListStoredMacrosCommandHandler() override;

    utils::void_result execute(
        const std::span<const std::string_view>& args,
        utils::commands::CommandResponseWriter& responseWriter,
        utils::allocations::Arena& arena
    ) override;

private:
    common::macros::MacroStorage& macroStorage;
};
