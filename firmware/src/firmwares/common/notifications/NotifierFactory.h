#pragma once

#include <memory>
#include <Print.h>

#include "Notifier.h"

class NotifierFactory {
public:
    explicit NotifierFactory(Print& outputStream);
    ~NotifierFactory();

    std::unique_ptr<Notifier> create(uint64_t deviceId) const;
private:
    Print& outputStream;
};
