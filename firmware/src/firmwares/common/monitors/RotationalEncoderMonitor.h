#pragma once

#include "utils/bitreaders/BitReader.h"
#include "utils/observables/Subject.h"

enum class RotationalEncoderDirection {
    Clockwise = 0x01,
    CounterClockwise = 0x02,
    None = 0xff,
};

struct EncoderRotatedEvent {
    uint8_t encoderNumber;
    RotationalEncoderDirection direction;
};

class RotationalEncoderMonitor {
public:
    RotationalEncoderMonitor(
        std::uint8_t encoderNumber,
        const std::shared_ptr<utils::bitreaders::BitReader>& aBitReader,
        const std::shared_ptr<utils::bitreaders::BitReader>& bBitReader
    );
    ~RotationalEncoderMonitor();

    utils::observables::Observable<EncoderRotatedEvent>& onEncoderRotated() const;

    std::uint8_t getEncoderNumber() const;

    void begin();
    void update();

private:
    std::uint8_t readState() const;

    const std::uint8_t encoderNumber;
    const std::shared_ptr<utils::bitreaders::BitReader> aBitReader;
    const std::shared_ptr<utils::bitreaders::BitReader> bBitReader;

    const std::unique_ptr<utils::observables::Subject<EncoderRotatedEvent>> encoderRotatedSubject;

    std::uint8_t previousState;
    RotationalEncoderDirection previousDirection = RotationalEncoderDirection::None;
};
