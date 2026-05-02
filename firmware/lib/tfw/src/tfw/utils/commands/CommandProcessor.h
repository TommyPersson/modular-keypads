#pragma once

#include <list>

#include "CommandHandler.h"
#include <tfw/utils/streams.h>
#include <tfw/utils/allocations.h>
#include <tfw/utils/observables.h>

namespace tfw::utils::commands {
    struct ParsedCommand {
        int id;
        std::string_view type;
        tfw::utils::allocations::arena::vector<std::string_view> args;
    };

    class CommandProcessor final : public observables::Observer<streams::LineReceivedEvent> {
    public:
        explicit CommandProcessor(Print& outputStream);
        ~CommandProcessor() override;

        void addHandler(const std::shared_ptr<CommandHandler>& handler);

        void observe(const streams::LineReceivedEvent& value) override;

    private:
        std::shared_ptr<CommandHandler> findHandler(const std::string_view& commandType);

        ParsedCommand parseCommand(const std::string_view& rawCommand);

        std::list<std::shared_ptr<CommandHandler>> handlers;
        Print& outputStream;
        tfw::utils::allocations::Arena arena;
    };
}
