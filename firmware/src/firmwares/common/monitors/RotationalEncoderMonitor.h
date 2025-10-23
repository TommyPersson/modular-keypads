#pragma once

#include "firmwares/common/bitreaders/BitReader.h"
#include "utils/observables/Subject.h"

enum class RotationalEncoderDirection {
    Clockwise,
    CounterClockwise,
    None,
};

struct EncoderRotatedEvent {
    uint8_t encoderNumber;
    RotationalEncoderDirection direction;
};

class RotationalEncoderMonitor {
public:
    RotationalEncoderMonitor(
        std::uint8_t encoderNumber,
        const std::shared_ptr<BitReader>& aBitReader,
        const std::shared_ptr<BitReader>& bBitReader
    );
    ~RotationalEncoderMonitor();

    Observable<EncoderRotatedEvent>& onEncoderRotated() const;

    std::uint8_t getEncoderNumber() const;

    void begin();
    void update();

private:
    std::uint8_t readState() const;

    const std::uint8_t encoderNumber;
    const std::shared_ptr<BitReader> aBitReader;
    const std::shared_ptr<BitReader> bBitReader;

    const std::unique_ptr<Subject<EncoderRotatedEvent>> encoderRotatedSubject;

    std::uint8_t previousState;
    RotationalEncoderDirection previousDirection = RotationalEncoderDirection::None;
};
