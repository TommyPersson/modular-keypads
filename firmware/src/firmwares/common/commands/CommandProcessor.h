#pragma once

#include <list>
#include <SerialPort/SerialPort.h>

#include "CommandHandler.h"
#include "firmwares/common/logging/Logger.h"
#include "firmwares/common/LineStreamer.h"
#include "utils/allocations/Arena.h"
#include "utils/observables/Observer.h"

struct ParsedCommand {
    int id;
    std::string_view type;
    arena::vector<std::string_view> args;
};

class CommandProcessor final : public Observer<LineEvent> {
public:
    explicit CommandProcessor(Print& outputStream, Logger& logger);
    ~CommandProcessor() override;

    void addHandler(const std::shared_ptr<CommandHandler>& handler);

    void observe(const LineEvent& value) override;

private:
    std::shared_ptr<CommandHandler> findHandler(const std::string_view& commandType);

    ParsedCommand parseCommand(const std::string_view& rawCommand);

    std::list<std::shared_ptr<CommandHandler>> handlers;
    Print& outputStream;
    Logger& logger;
    Arena arena;
};
