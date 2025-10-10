#include "PingCommandHandler.h"

PingCommandHandler::PingCommandHandler(Logger& logger) :
    CommandHandler("ping", logger) {
}

PingCommandHandler::~PingCommandHandler() = default;

std::string PingCommandHandler::execute(const std::span<const std::string_view>& args) {
    for (auto& arg : args) {
        logger.debug("arg = %.*s", arg.length(), arg.data());
    }
    return "pong";
}
