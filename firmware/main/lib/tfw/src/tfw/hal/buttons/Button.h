#pragma once

#include <memory>

#include <tfw/hal/bitreaders.h>
#include <tfw/hal/time.h>
#include <tfw/utils/observables.h>

namespace tfw::hal::buttons {

    enum class ButtonState : uint8_t {
        PRESSED = 0x01,
        UNPRESSED = 0x02,
        // Still pressed, after being held for a while
        LONG_PRESSED = 0x03,
        UNKNOWN = 0xff,
    };

    struct ButtonStateChangedEvent {
        uint8_t buttonNumber;
        ButtonState fromState;
        ButtonState toState;
    };

    // Same as the SMC buttons
    inline constexpr uint64_t DEFAULT_LONG_PRESS_US = 500'000;

    /**
     * A button that is polled through update(). Pressing it changes its state to PRESSED, and holding it for
     * longPressUs then changes it to LONG_PRESSED, until it's released.
     */
    class Button {
    public:
        Button(
            std::uint8_t number,
            const std::shared_ptr<bitreaders::BitReader>& bitReader,
            const time::Clock& clock,
            uint64_t longPressUs = DEFAULT_LONG_PRESS_US
        );
        Button(const Button&) = delete;
        ~Button();

        void begin();
        void update();

        utils::observables::Observable<ButtonStateChangedEvent>& onStateChanged() const;

        std::uint8_t getNumber() const;
        ButtonState getCurrentState() const;
        ButtonState getPreviousState() const;

    private:
        ButtonState readState() const;
        void changeState(ButtonState state);

        const std::uint8_t number;
        const std::shared_ptr<bitreaders::BitReader> bitReader;

        std::unique_ptr<utils::observables::Subject<ButtonStateChangedEvent>> buttonStateChangedSubject;
        std::unique_ptr<time::CountDown> longPressCountDown;

        ButtonState currentState = ButtonState::UNKNOWN;
        ButtonState previousState = ButtonState::UNKNOWN;
    };
}
