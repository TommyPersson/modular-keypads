#include "Notifier.h"

using namespace mkp::components::notifications;

Notifier::Notifier(Print& outputStream) :
    outputStream(outputStream) {
}

Notifier::~Notifier() = default;

void Notifier::notify(const uint64_t deviceId, const Notification& notification) const {
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
