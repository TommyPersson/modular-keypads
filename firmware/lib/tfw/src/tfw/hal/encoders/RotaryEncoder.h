#pragma once

#include <tfw/hal/bitreaders.h>
#include <tfw/utils/observables.h>

namespace tfw::hal::encoders {
    enum class RotaryEncoderDirection {
        Clockwise = 0x01,
        CounterClockwise = 0x02,
        None = 0xff,
    };

    struct EncoderRotatedEvent {
        uint8_t encoderNumber;
        RotaryEncoderDirection direction;
    };

    class RotaryEncoder {
    public:
        RotaryEncoder(
            std::uint8_t encoderNumber,
            const std::shared_ptr<bitreaders::BitReader>& aBitReader,
            const std::shared_ptr<bitreaders::BitReader>& bBitReader
        );
        ~RotaryEncoder();

        utils::observables::Observable<EncoderRotatedEvent>& onEncoderRotated() const;

        std::uint8_t getEncoderNumber() const;

        void begin();
        void update();

    private:
        std::uint8_t readState() const;

        const std::uint8_t encoderNumber;
        const std::shared_ptr<bitreaders::BitReader> aBitReader;
        const std::shared_ptr<bitreaders::BitReader> bBitReader;

        const std::unique_ptr<utils::observables::Subject<EncoderRotatedEvent>> encoderRotatedSubject;

        std::uint8_t previousState;
        RotaryEncoderDirection previousDirection = RotaryEncoderDirection::None;
    };
}
