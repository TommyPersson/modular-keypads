#pragma once

#include <memory>
#include <Print.h>

#include "Notifier.h"

namespace mkp::components::notifications {
    class NotifierFactory {
    public:
        explicit NotifierFactory(Print& outputStream);
        ~NotifierFactory();

        std::unique_ptr<Notifier> create(uint64_t deviceId) const;
    private:
        Print& outputStream;
    };
}