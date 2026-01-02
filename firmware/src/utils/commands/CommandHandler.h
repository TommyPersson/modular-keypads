#pragma once

#include <string>
#include <string_view>
#include <span>
#include <utility>

#include "utils/allocations/Arena.h"
#include "utils/result.h"

#include "CommandResponseWriter.h"


class CommandHandler {
public:
    explicit CommandHandler(std::string commandType);
    virtual ~CommandHandler() = default;

    const std::string& getCommandType() const {
        return this->commandType;
    };

    virtual utils::void_result execute(
        const std::span<const std::string_view>& args,
        CommandResponseWriter& responseWriter,
        Arena& arena
    ) = 0;

private:
    std::string commandType;
};

inline CommandHandler::CommandHandler(std::string commandType)
    : commandType(std::move(commandType)) {
}
