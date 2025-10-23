#include "PingCommandHandler.h"

PingCommandHandler::PingCommandHandler(Logger& logger) :
    CommandHandler("ping", logger) {
}

PingCommandHandler::~PingCommandHandler() = default;

void PingCommandHandler::execute(
    const std::span<const std::string_view>& args,
    CommandResponseWriter& responseWriter,
    Arena& arena
    ) {
    for (auto& arg : args) {
        logger.debug("arg = %.*s", arg.length(), arg.data());
    }

    responseWriter.writeLine("pong");
}
