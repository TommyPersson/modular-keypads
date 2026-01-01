#include "PingCommandHandler.h"


namespace {
    auto logger = common::logging::createLogger("PingCommandHandler");
}

PingCommandHandler::PingCommandHandler() :
    CommandHandler("ping") {
}

PingCommandHandler::~PingCommandHandler() = default;

utils::void_result PingCommandHandler::execute(
    const std::span<const std::string_view>& args,
    CommandResponseWriter& responseWriter,
    Arena& arena
    ) {
    for (auto& arg : args) {
        logger->debug("arg = %.*s", arg.length(), arg.data());
    }

    responseWriter.writeLine("pong");

    return utils::void_result::success();
}
