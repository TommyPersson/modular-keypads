#pragma once

#include "firmwares/common/commands/CommandHandler.h"
#include "firmwares/common/macros/MacroStorage.h"

class ListStoredMacrosCommandHandler : public CommandHandler {
public:
    explicit ListStoredMacrosCommandHandler(common::macros::MacroStorage& macroStorage);
    ~ListStoredMacrosCommandHandler() override;

    utils::void_result execute(
        const std::span<const std::string_view>& args,
        CommandResponseWriter& responseWriter,
        Arena& arena
    ) override;

private:
    common::macros::MacroStorage& macroStorage;
};
