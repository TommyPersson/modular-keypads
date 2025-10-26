#pragma once

#include <Print.h>

#include "Notification.h"

class Notifier {
public:
    explicit Notifier(Print& outputStream);
    ~Notifier();

    void begin(const std::string& deviceId);
    void notify(const Notification& notification);

private:
    Print& outputStream;
    std::string deviceId;
};
