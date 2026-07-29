#pragma once

#include <Print.h>

#include "Notification.h"

class Notifier {
public:
    explicit Notifier(uint64_t deviceId, Print& outputStream);
    ~Notifier();

    void notify(const Notification& notification) const;

private:
    Print& outputStream;
    uint64_t deviceId;
};
