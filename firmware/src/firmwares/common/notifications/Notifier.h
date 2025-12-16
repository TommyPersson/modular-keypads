#pragma once

#include <Print.h>

#include "Notification.h"

class Notifier {
public:
    explicit Notifier(const std::string& deviceId, Print& outputStream);
    ~Notifier();

    void notify(const Notification& notification) const;

private:
    Print& outputStream;
    std::string deviceId;
};
