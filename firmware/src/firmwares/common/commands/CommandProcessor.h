#pragma once

#include <list>
#include <WString.h>
#include <SerialPort/SerialPort.h>
#include "utils/observables/Observer.h"

#include "CommandHandler.h"

class CommandProcessor final : public Observer<String> {
public:
    explicit CommandProcessor(Stream& outputStream);
    ~CommandProcessor() override;

    void addHandler(const std::shared_ptr<CommandHandler>& handler);

    void observe(const String& value) override;

private:
    std::list<std::shared_ptr<CommandHandler>> handlers;
    Stream& outputStream;
};

