#include "Button.h"

#include <tfw/hal/logging.h>

using namespace tfw::hal::buttons;
using namespace tfw::hal::bitreaders;
using namespace tfw::utils::observables;

Button::Button(
    const std::uint8_t number,
    const std::shared_ptr<BitReader>& bitReader
) : number(number),
    bitReader(bitReader) {
    buttonStateChangedSubject = std::make_unique<Subject<ButtonStateChangedEvent>>();
}

Button::~Button() = default;

void Button::begin() {
    previousState = readState();
    currentState = previousState;
}

void Button::update() {
    const auto nextState = readState();

    previousState = currentState;
    currentState = nextState;

    if (currentState != previousState) {
        buttonStateChangedSubject->notify(
            {
                .buttonNumber = number,
                .fromState = previousState,
                .toState = currentState,
            }
        );
    }
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
