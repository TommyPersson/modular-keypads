#include "Button.h"

#include <tfw/hal/logging.h>

using namespace tfw::hal::buttons;
using namespace tfw::hal::bitreaders;
using namespace tfw::utils::observables;

Button::Button(
    const std::uint8_t number,
    const std::shared_ptr<BitReader>& bitReader,
    const time::Clock& clock,
    const uint64_t longPressUs
) : number(number),
    bitReader(bitReader) {
    buttonStateChangedSubject = std::make_unique<Subject<ButtonStateChangedEvent>>();
    longPressCountDown = clock.createCountDown(longPressUs, [this] {
        changeState(ButtonState::LONG_PRESSED);
    });
}

Button::~Button() = default;

void Button::begin() {
    previousState = readState();
    currentState = previousState;
}

void Button::update() {
    const auto nextState = readState();

    // A long pressed button reads as pressed
    const auto isUnchanged = nextState == currentState
        || (nextState == ButtonState::PRESSED && currentState == ButtonState::LONG_PRESSED);

    if (isUnchanged) {
        longPressCountDown->update();
        return;
    }

    if (nextState == ButtonState::PRESSED) {
        longPressCountDown->start();
    } else {
        longPressCountDown->stop();
    }

    changeState(nextState);
}

void Button::changeState(const ButtonState state) {
    previousState = currentState;
    currentState = state;

    buttonStateChangedSubject->notify(
        {
            .buttonNumber = number,
            .fromState = previousState,
            .toState = currentState,
        }
    );
}

Observable<ButtonStateChangedEvent>& Button::onStateChanged() const {
    return *buttonStateChangedSubject;
}

std::uint8_t Button::getNumber() const {
    return number;
}

ButtonState Button::getCurrentState() const {
    return currentState;
}

ButtonState Button::getPreviousState() const {
    return previousState;
}

ButtonState Button::readState() const {
    return bitReader->read() ? ButtonState::PRESSED : ButtonState::UNPRESSED;
}
