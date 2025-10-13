#pragma once

#include <list>
#include <SerialPort/SerialPort.h>

#include "../logging/Logger.h"
#include "utils/allocations/Arena.h"
#include "utils/observables/Observer.h"

#include "CommandHandler.h"

struct ParsedCommand {
    int id;
    std::string_view type;
    arena::vector<std::string_view> args;
};

class CommandProcessor final : public Observer<std::string> {
public:
    explicit CommandProcessor(Stream& outputStream, Logger& logger);
    ~CommandProcessor() override;

    void addHandler(const std::shared_ptr<CommandHandler>& handler);

    void observe(const std::string& value) override;

private:
    std::shared_ptr<CommandHandler> findHandler(const std::string_view& commandType);

    ParsedCommand parseCommand(const std::string_view& rawCommand);

    std::list<std::shared_ptr<CommandHandler>> handlers;
    Stream& outputStream;
    Logger& logger;
    Arena arena;
};
