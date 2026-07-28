#include "PingCommandHandler.h"

#include <tfw/hal/logging.h>


namespace {
    auto logger = tfw::hal::logging::createLogger("PingCommandHandler");
}

PingCommandHandler::PingCommandHandler() :
    CommandHandler("ping") {
}

PingCommandHandler::~PingCommandHandler() = default;

tfw::utils::void_result PingCommandHandler::execute(
    const std::span<const std::string_view>& args,
    tfw::utils::commands::CommandResponseWriter& responseWriter,
    tfw::utils::allocations::Arena& arena
    ) {
    for (auto& arg : args) {
        logger->debug("arg = %.*s", arg.length(), arg.data());
    }

    responseWriter.writeLine("pong");

    return tfw::utils::void_result::success();
}
