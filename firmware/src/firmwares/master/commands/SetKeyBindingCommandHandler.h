#pragma once

#include "utils/commands/CommandHandler.h"
#include "firmwares/common/keybindings/KeyBindingStorage.h"

class SetKeyBindingCommandHandler : public CommandHandler {
public:
    explicit SetKeyBindingCommandHandler(common::keybindings::KeyBindingStorage& keyBindingStorage);
    ~SetKeyBindingCommandHandler() override;

    utils::void_result execute(
        const std::span<const std::string_view>& args,
        CommandResponseWriter& responseWriter,
        Arena& arena
    ) override;

private:
    common::keybindings::KeyBindingStorage& keyBindingStorage;
};
