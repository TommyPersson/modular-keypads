#pragma once

#include <string>
#include <string_view>
#include <span>
#include <utility>
#include <firmwares/common/Logger.h>


class CommandHandler {
public:
    explicit CommandHandler(std::string commandType, Logger& logger);
    virtual ~CommandHandler() = default;

    const std::string& getCommandType() const {
        return this->commandType;
    };

    virtual std::string execute(const std::span<const std::string_view>& args) = 0;

protected:
    Logger& logger;

private:
    std::string commandType;
};

inline CommandHandler::CommandHandler(std::string commandType, Logger& logger)
    : logger(logger),
      commandType(std::move(commandType)) {
}
