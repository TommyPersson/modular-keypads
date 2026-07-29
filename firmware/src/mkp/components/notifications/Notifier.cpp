#include "Notifier.h"

Notifier::Notifier(const uint64_t deviceId, Print& outputStream) :
    deviceId(deviceId), outputStream(outputStream) {
}

Notifier::~Notifier() = default;

void Notifier::notify(const Notification& notification) const {
    outputStream.print("!");
    outputStream.printf("%08llx", deviceId);
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
