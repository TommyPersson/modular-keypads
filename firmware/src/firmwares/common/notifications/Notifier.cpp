#include "Notifier.h"

Notifier::Notifier(const std::string& deviceId, Print& outputStream) :
    deviceId(deviceId), outputStream(outputStream) {
}

Notifier::~Notifier() = default;

void Notifier::notify(const Notification& notification) const {
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
