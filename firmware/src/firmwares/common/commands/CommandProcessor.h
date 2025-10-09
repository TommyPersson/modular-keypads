#pragma once

#include <list>
#include <SerialPort/SerialPort.h>

#include "firmwares/common/Logger.h"
#include "utils/allocations/Arena.h"
#include "utils/observables/Observer.h"

#include "CommandHandler.h"

class CommandProcessor final : public Observer<std::string> {
public:
    explicit CommandProcessor(Stream& outputStream, Logger& logger);
    ~CommandProcessor() override;

    void addHandler(const std::shared_ptr<CommandHandler>& handler);

    void observe(const std::string& value) override;

private:
    std::list<std::shared_ptr<CommandHandler>> handlers;
    Stream& outputStream;
    Logger& logger;
    Arena arena;
};

