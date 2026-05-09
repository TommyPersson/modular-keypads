#pragma once

#include <memory>

#include <tfw/hal/bitreaders.h>
#include <tfw/utils/observables.h>

namespace tfw::hal::buttons {

    enum class ButtonState {
        PRESSED,
        UNPRESSED,
        UNKNOWN,
    };

    struct ButtonStateChangedEvent {
        uint8_t buttonNumber;
        ButtonState fromState;
        ButtonState toState;
    };

    class Button {
    public:
        Button(std::uint8_t number, const std::shared_ptr<bitreaders::BitReader>& bitReader);
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

        const std::uint8_t number;
        const std::shared_ptr<bitreaders::BitReader> bitReader;

        std::unique_ptr<utils::observables::Subject<ButtonStateChangedEvent>> buttonStateChangedSubject;

        ButtonState currentState = ButtonState::UNKNOWN;
        ButtonState previousState = ButtonState::UNKNOWN;
    };
}
