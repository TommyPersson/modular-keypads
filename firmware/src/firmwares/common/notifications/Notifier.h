#pragma once

#include <Print.h>

#include "Notification.h"

class Notifier {
public:
    explicit Notifier(Print& outputStream);
    ~Notifier();

    void notify(const Notification& notification);

private:
    Print& outputStream;
};
