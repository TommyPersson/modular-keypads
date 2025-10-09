#pragma once

#include <string>
#include <string_view>
#include <utility>


class CommandHandler {
public:
    explicit CommandHandler(std::string commandType);
    virtual ~CommandHandler() = default;

    const std::string& getCommandType() const {
        return this->commandType;
    };

    virtual std::string execute() = 0;

private:
    std::string commandType;
};

inline CommandHandler::CommandHandler(std::string commandType) : commandType(std::move(commandType)) {
}
