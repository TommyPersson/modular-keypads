#pragma once

#include <Print.h>

#include "Notification.h"

namespace mkp::components::notifications {
    class Notifier {
    public:
        explicit Notifier(Print& outputStream);
        ~Notifier();

        void notify(uint64_t deviceId, const Notification& notification) const;

    private:
        Print& outputStream;
    };
}