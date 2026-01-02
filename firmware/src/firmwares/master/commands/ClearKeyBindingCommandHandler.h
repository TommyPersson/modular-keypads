#pragma once

#include "utils/commands/CommandHandler.h"
#include "firmwares/common/keybindings/KeyBindingStorage.h"

class ClearKeyBindingCommandHandler : public utils::commands::CommandHandler {
public:
    explicit ClearKeyBindingCommandHandler(common::keybindings::KeyBindingStorage& keyBindingStorage);
    ~ClearKeyBindingCommandHandler() override;

    utils::void_result execute(
        const std::span<const std::string_view>& args,
        utils::commands::CommandResponseWriter& responseWriter,
        utils::allocations::Arena& arena
    ) override;

private:
    common::keybindings::KeyBindingStorage& keyBindingStorage;
};
