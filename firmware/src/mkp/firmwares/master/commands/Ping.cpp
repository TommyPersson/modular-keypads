#include "Ping.h"

#include <tfw/hal/logging.h>


namespace {
    auto logger = tfw::hal::logging::createLogger("PingCommandHandler");
}

using namespace mkp::firmwares::master::commands;

Ping::Ping() :
    CommandHandler("ping") {
}

Ping::~Ping() = default;

tfw::utils::void_result Ping::execute(
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
