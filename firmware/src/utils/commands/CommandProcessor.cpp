#include "CommandProcessor.h"

#include <span>

#include "utils/strings.h"
#include "utils/allocations/ArenaUtils.h"
#include "utils/logging/Logger.h"

namespace {
    auto logger = utils::logging::createLogger("CommandProcessor");
}

using namespace utils::commands;
using namespace utils::streams;

CommandProcessor::CommandProcessor(Print& outputStream) :
    outputStream(outputStream),
    arena(4096) {
}

CommandProcessor::~CommandProcessor() {
    this->handlers.clear();
}

void CommandProcessor::addHandler(const std::shared_ptr<CommandHandler>& handler) {
    this->handlers.push_back(handler);
}

void CommandProcessor::observe(const LineReceivedEvent& value) {
    const auto command = parseCommand(value.text);

    const auto handler = findHandler(command.type);
    if (handler != nullptr) {
        auto responseWriter = CommandResponseWriter(command.id, outputStream);
        auto result = handler->execute(command.args, responseWriter, this->arena);
        if (result.has_error) {
            logger->error(
                "Execution error for '%.*s' (%i): '%s'",
                command.type.length(),
                command.type.data(),
                command.id,
                result.error_code
            );
            outputStream.printf("%%%i.e:%s\n", command.id, result.error_code);
        } else {
            outputStream.printf("%%%i.0\n", command.id);
        }
    } else {
        logger->error("unknown.command: %.*s", command.type.length(), command.type.data());
    }

    arena.reset();
}

ParsedCommand CommandProcessor::parseCommand(const std::string_view& rawCommand) {
    utils::allocations::ArenaAllocator<std::string_view> allocator(this->arena);

    const auto commandParts = utils::allocations::arena::strings::split(rawCommand, ':', allocator);

    const auto commandId = strings::atol(commandParts[0]);
    const auto command = commandParts[1];
    auto args = commandParts.size() > 2
                    ? utils::allocations::arena::strings::split(commandParts[2], ',', allocator)
                    : utils::allocations::arena::vector(allocator);

    return ParsedCommand{
        .id = commandId,
        .type = command,
        .args = std::move(args),
    };
}

std::shared_ptr<CommandHandler> CommandProcessor::findHandler(const std::string_view& commandType) {
    for (auto& handler : handlers) {
        if (handler->getCommandType() == commandType) {
            return handler;
        }
    }

    return nullptr;
}
