#pragma once

#include <string>
#include <string_view>
#include <span>
#include <utility>

#include <tfw/utils/allocations.h>
#include <tfw/utils/result.h>

#include "CommandResponseWriter.h"

namespace tfw::utils::commands {
    class CommandHandler {
    public:
        explicit CommandHandler(std::string commandType) : commandType(std::move(commandType)) {
        }

        virtual ~CommandHandler() = default;

        const std::string& getCommandType() const {
            return this->commandType;
        };

        virtual void_result execute(
            const std::span<const std::string_view>& args,
            CommandResponseWriter& responseWriter,
            tfw::utils::allocations::Arena& arena
        ) = 0;

    private:
        std::string commandType;
    };
}
