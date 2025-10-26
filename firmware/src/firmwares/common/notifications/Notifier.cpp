#include "Notifier.h"

Notifier::Notifier(Print& outputStream) :
    outputStream(outputStream) {
}

Notifier::~Notifier() = default;

void Notifier::begin(const std::string& deviceId) {
    this->deviceId = deviceId;
}

void Notifier::notify(const Notification& notification) {
    outputStream.print("!");
    outputStream.print(deviceId.c_str());
    outputStream.print(":");
    outputStream.print(notification.type.c_str());
    outputStream.print(":");

    bool isFirst = true;
    for (auto& arg : notification.args) {
        if (isFirst) {
            isFirst = false;
        } else {
            outputStream.print(",");
        }

        outputStream.print(arg.c_str());
    }

    outputStream.print("\n");
    outputStream.flush();
    outputStream.flush();
}
