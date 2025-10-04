#include <utility>

#pragma once

class CommandHandler {
public:
    explicit CommandHandler(String commandType);
    virtual ~CommandHandler() = default;

    const String& getCommandType() const {
        return this->commandType;
    };

    virtual String execute() = 0;

private:
    String commandType;
};

inline CommandHandler::CommandHandler(String commandType) : commandType(std::move(commandType)) {
}
