#include "EncoderRotationNotifier.h"


EncoderRotationNotifier::EncoderRotationNotifier(Notifier& notifier) :
    notifier(notifier) {
}

EncoderRotationNotifier::~EncoderRotationNotifier() = default;

void EncoderRotationNotifier::observe(const EncoderRotatedEvent& event) {
    if (event.direction == RotationalEncoderDirection::Clockwise) {
        notifier.notify(
            {
                .type = "encoder.rotated.clockwise",
                .args = {std::to_string(event.encoderNumber)}
            }
            );
    } else if (event.direction == RotationalEncoderDirection::CounterClockwise) {
        notifier.notify(
            {
                .type = "encoder.rotated.counter-clockwise",
                .args = {std::to_string(event.encoderNumber)}
            }
            );
    }
}
