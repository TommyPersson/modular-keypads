#include "CommandProcessor.h"

#include "utils/strings.h"

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

std::shared_ptr<CommandHandler> findHandler(
    const std::string_view& commandType,
    const std::list<std::shared_ptr<CommandHandler>>& handlers
    ) {
    for (auto& handler : handlers) {
        if (handler->getCommandType() == commandType) {
            return handler;
        }
    }

    return nullptr;
}

void CommandProcessor::observe(const std::string& value) {
    const auto line = std::string_view(value);

    int commandIdLength;
    const int commandId = utils::strings::atol(line, commandIdLength);
    const auto command = line.substr(commandIdLength+1);

    const auto handler = findHandler(command, this->handlers);

    if (handler != nullptr) {
        const auto response = handler->execute();
        outputStream.printf("%%%i:%s\n", commandId, response.c_str());
    } else {
        logger.error("unknown.command: %.*s", command.length(), command.data());
    }

    arena.reset();
}
