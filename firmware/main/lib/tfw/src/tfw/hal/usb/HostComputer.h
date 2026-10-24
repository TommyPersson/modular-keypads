#pragma once

#include <cstdint>

namespace tfw::hal::usb {
    // The computer on the other end of the USB connection, as reached through the HID devices: keyboard, consumer
    // control, system control and mouse, and the USB MIDI device. Separated from the USB connection, so that logic sending input can be tested.
    class HostComputer {
    public:
        virtual ~HostComputer() = default;

        // HID key codes, including the modifier keys 0xe0 to 0xe7
        virtual void pressKey(uint8_t keyCode) = 0;
        virtual void releaseKey(uint8_t keyCode) = 0;
        virtual void releaseAllKeys() = 0;

        // Presses and releases the keys that type the character
        virtual void typeCharacter(char character) = 0;

        virtual void pressConsumerControl(uint16_t usageId) = 0;
        virtual void releaseConsumerControl() = 0;

        virtual void pressSystemControl(uint8_t code) = 0;
        virtual void releaseSystemControl() = 0;

        // Presses and releases the buttons, as bit flags: left, right, middle, back, forward
        virtual void clickMouse(uint8_t buttons) = 0;
        // Relative movement and scrolling. Positive wheel scrolls up, positive pan scrolls right.
        virtual void moveMouse(int8_t x, int8_t y, int8_t wheel, int8_t pan) = 0;

        // Channels 1 to 16
        virtual void sendMidiNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) = 0;
        virtual void sendMidiNoteOff(uint8_t channel, uint8_t note) = 0;
        virtual void sendMidiControlChange(uint8_t channel, uint8_t controller, uint8_t value) = 0;
    };
}
