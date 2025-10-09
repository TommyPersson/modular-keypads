#include "CommandProcessor.h"

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
};


std::shared_ptr<CommandHandler> findHandler(
    const String& commandType,
    std::list<std::shared_ptr<CommandHandler>>& handlers
    ) {
    for (auto& handler : handlers) {
        if (handler->getCommandType().equals(commandType)) {
            return handler;
        }
    }

    return nullptr;
}

void CommandProcessor::observe(const String& value) {
    unsigned int bufferSize = value.length();
    auto commandIdBuffer = reinterpret_cast<char*>(arena.allocate(bufferSize));

    auto line = value;
    int commandId = 0;
    String command;

    memset(commandIdBuffer, 0, bufferSize);
    for (int i = 0; i < line.length() - 1; i++) {
        auto c = line[i];
        if (isdigit(c)) {
            commandIdBuffer[i] = c;
        } else {
            commandIdBuffer[i] = 0;
            break;
        }
    }

    commandId = atoi(commandIdBuffer);
    command = line.substring(strlen(commandIdBuffer) + 1);

    const auto handler = findHandler(command, this->handlers);

    if (handler != nullptr) {
        const auto response = handler->execute();
        outputStream.printf("%%%i:%s\n", commandId, response.c_str());
    } else {
        logger.error("unknown.command: %s", command.c_str());
    }

    arena.reset();
}
