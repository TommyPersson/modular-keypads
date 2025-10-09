#include "PingCommandHandler.h"

PingCommandHandler::PingCommandHandler() :
    CommandHandler("ping") {
}

PingCommandHandler::~PingCommandHandler() = default;

std::string PingCommandHandler::execute() {
    return "pong";
}
