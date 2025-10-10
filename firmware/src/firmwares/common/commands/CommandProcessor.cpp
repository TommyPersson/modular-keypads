#include "CommandProcessor.h"

#include <span>

#include "utils/strings.h"
#include "utils/allocations/ArenaUtils.h"


CommandProcessor::CommandProcessor(Stream& outputStream, Logger& logger) :
    outputStream(outputStream),
    logger(logger),
    arena(4096) {
}

CommandProcessor::~CommandProcessor() {
    this->handlers.clear();
}

void CommandProcessor::addHandler(const std::shared_ptr<CommandHandler>& handler) {
    this->handlers.push_back(handler);
}

void CommandProcessor::observe(const std::string& value) {
    const auto command = parseCommand(value);

    const auto handler = findHandler(command.type);

    if (handler != nullptr) {
        const auto response = handler->execute(command.args);
        outputStream.printf("%%%i:%s\n", command.id, response.c_str());
    } else {
        logger.error("unknown.command: %.*s", command.type.length(), command.type.data());
    }

    arena.reset();
}

ParsedCommand CommandProcessor::parseCommand(const std::string_view& rawCommand) {
    ArenaAllocator<std::string_view> allocator(this->arena, this->logger);

    const auto commandParts = arena::strings::split(rawCommand, ':', allocator);

    const auto commandId = utils::strings::atol(commandParts[0]);
    const auto command = commandParts[1];
    auto args = commandParts.size() > 2
        ? arena::strings::split(commandParts[2], ',', allocator)
        : arena::vector(allocator);

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
