#include "RotationalEncoderMonitor.h"

RotationalEncoderMonitor::RotationalEncoderMonitor(
    const std::uint8_t encoderNumber,
    const std::shared_ptr<BitReader>& aBitReader,
    const std::shared_ptr<BitReader>& bBitReader
    ) :
    encoderNumber(encoderNumber),
    aBitReader(aBitReader),
    bBitReader(bBitReader),
    encoderRotatedSubject(std::make_unique<Subject<EncoderRotatedEvent>>()),
    previousState(0),
    lastUpdateTime(0) {
}

RotationalEncoderMonitor::~RotationalEncoderMonitor() = default;

Observable<EncoderRotatedEvent>& RotationalEncoderMonitor::onEncoderRotated() const {
    return *encoderRotatedSubject;
}

std::uint8_t RotationalEncoderMonitor::getEncoderNumber() const {
    return encoderNumber;
}

void RotationalEncoderMonitor::begin() {
    previousState = readState();
}

void RotationalEncoderMonitor::update() {
    const auto newState = readState();

    if (newState == previousState) {
        if (newState == 0b11 || newState == 0b00) {
            if (previousDirection != RotationalEncoderDirection::None) {
                const auto direction = previousDirection;
                previousDirection = RotationalEncoderDirection::None;

                encoderRotatedSubject->notify({
                        .encoderNumber = encoderNumber,
                        .direction = direction,
                    }
                    );
            }
        }

        return;
    }

    switch (newState) {
    case 0b00:
        if (previousState == 0b01) {
            previousDirection = RotationalEncoderDirection::Clockwise;
        } else if (previousState == 0b10) {
            previousDirection = RotationalEncoderDirection::CounterClockwise;
        }
        break;
    case 0b01:
        if (previousState == 0b11) {
            previousDirection = RotationalEncoderDirection::Clockwise;
        } else if (previousState == 0b00) {
            previousDirection = RotationalEncoderDirection::CounterClockwise;
        }
        break;
    case 0b10:
        if (previousState == 0b00) {
            previousDirection = RotationalEncoderDirection::Clockwise;
        } else if (previousState == 0b11) {
            previousDirection = RotationalEncoderDirection::CounterClockwise;
        }
        break;
    case 0b11:
        if (previousState == 0b10) {
            previousDirection = RotationalEncoderDirection::Clockwise;
        } else if (previousState == 0b01) {
            previousDirection = RotationalEncoderDirection::CounterClockwise;
        }
        break;
    default:
        break;
    }

    previousState = newState;
}

std::uint8_t RotationalEncoderMonitor::readState() const {
    return aBitReader->read() << 1 | bBitReader->read();
}
