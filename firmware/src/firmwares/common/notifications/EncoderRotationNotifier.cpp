#include "EncoderRotationNotifier.h"

using namespace tfw::hal::encoders;

EncoderRotationNotifier::EncoderRotationNotifier(Notifier& notifier) :
    notifier(notifier) {
}

EncoderRotationNotifier::~EncoderRotationNotifier() = default;

void EncoderRotationNotifier::observe(const EncoderRotatedEvent& event) {
    if (event.direction == RotaryEncoderDirection::Clockwise) {
        notifier.notify(
            {
                .type = "encoder.rotated.clockwise",
                .args = {std::to_string(event.encoderNumber)}
            }
            );
    } else if (event.direction == RotaryEncoderDirection::CounterClockwise) {
        notifier.notify(
            {
                .type = "encoder.rotated.counter-clockwise",
                .args = {std::to_string(event.encoderNumber)}
            }
            );
    }
}
