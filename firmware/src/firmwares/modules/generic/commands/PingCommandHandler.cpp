#include "PingCommandHandler.h"

PingCommandHandler::PingCommandHandler() :
    CommandHandler("ping") {
}

PingCommandHandler::~PingCommandHandler() = default;

String PingCommandHandler::execute() {
    return "pong";
}
