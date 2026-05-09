#include "RotaryEncoder.h"

using namespace tfw::hal::encoders;
using namespace tfw::hal::bitreaders;
using namespace tfw::utils::observables;

RotaryEncoder::RotaryEncoder(
    const std::uint8_t encoderNumber,
    const std::shared_ptr<BitReader>& aBitReader,
    const std::shared_ptr<BitReader>& bBitReader
) :
    encoderNumber(encoderNumber),
    aBitReader(aBitReader),
    bBitReader(bBitReader),
    encoderRotatedSubject(std::make_unique<Subject<EncoderRotatedEvent>>()),
    previousState(0) {
}

RotaryEncoder::~RotaryEncoder() = default;

Observable<EncoderRotatedEvent>& RotaryEncoder::onEncoderRotated() const {
    return *encoderRotatedSubject;
}

std::uint8_t RotaryEncoder::getEncoderNumber() const {
    return encoderNumber;
}

void RotaryEncoder::begin() {
    previousState = readState();
}

void RotaryEncoder::update() {
    const auto newState = readState();

    if (newState == previousState) {
        if (newState == 0b11 || newState == 0b00) {
            if (previousDirection != RotaryEncoderDirection::None) {
                const auto direction = previousDirection;
                previousDirection = RotaryEncoderDirection::None;

                encoderRotatedSubject->notify(
                    {
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
            previousDirection = RotaryEncoderDirection::Clockwise;
        } else if (previousState == 0b10) {
            previousDirection = RotaryEncoderDirection::CounterClockwise;
        }
        break;
    case 0b01:
        if (previousState == 0b11) {
            previousDirection = RotaryEncoderDirection::Clockwise;
        } else if (previousState == 0b00) {
            previousDirection = RotaryEncoderDirection::CounterClockwise;
        }
        break;
    case 0b10:
        if (previousState == 0b00) {
            previousDirection = RotaryEncoderDirection::Clockwise;
        } else if (previousState == 0b11) {
            previousDirection = RotaryEncoderDirection::CounterClockwise;
        }
        break;
    case 0b11:
        if (previousState == 0b10) {
            previousDirection = RotaryEncoderDirection::Clockwise;
        } else if (previousState == 0b01) {
            previousDirection = RotaryEncoderDirection::CounterClockwise;
        }
        break;
    default:
        break;
    }

    previousState = newState;
}

std::uint8_t RotaryEncoder::readState() const {
    return aBitReader->read() << 1 | bBitReader->read();
}
