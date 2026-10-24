#include "RealHostComputer.h"

#include "USB.h"

#ifdef SOC_USB_OTG_SUPPORTED

#include <USBHIDConsumerControl.h>
#include <USBHIDKeyboard.h>
#include <USBHIDMouse.h>
#include <USBHIDSystemControl.h>
#include <USBMIDI.h>

namespace {
    // Global, as the devices register with the USB stack when constructed, which has to happen before it's started
    USBHIDKeyboard keyboard;
    USBHIDConsumerControl consumerControl;
    USBHIDSystemControl systemControl;
    USBHIDMouse mouse;
    USBMIDI midi;
}

void tfw::hal::usb::RealHostComputer::begin() {
    keyboard.begin();
    consumerControl.begin();
    systemControl.begin();
    mouse.begin();
    midi.begin();
}

void tfw::hal::usb::RealHostComputer::pressKey(const uint8_t keyCode) {
    keyboard.pressRaw(keyCode);
}

void tfw::hal::usb::RealHostComputer::releaseKey(const uint8_t keyCode) {
    keyboard.releaseRaw(keyCode);
}

void tfw::hal::usb::RealHostComputer::releaseAllKeys() {
    keyboard.releaseAll();
}

void tfw::hal::usb::RealHostComputer::typeCharacter(const char character) {
    keyboard.write(character);
}

void tfw::hal::usb::RealHostComputer::pressConsumerControl(const uint16_t usageId) {
    consumerControl.press(usageId);
}

void tfw::hal::usb::RealHostComputer::releaseConsumerControl() {
    consumerControl.release();
}

void tfw::hal::usb::RealHostComputer::pressSystemControl(const uint8_t code) {
    systemControl.press(code);
}

void tfw::hal::usb::RealHostComputer::releaseSystemControl() {
    systemControl.release();
}

void tfw::hal::usb::RealHostComputer::clickMouse(const uint8_t buttons) {
    mouse.click(buttons);
}

void tfw::hal::usb::RealHostComputer::moveMouse(const int8_t x, const int8_t y, const int8_t wheel, const int8_t pan) {
    mouse.move(x, y, wheel, pan);
}

void tfw::hal::usb::RealHostComputer::sendMidiNoteOn(const uint8_t channel, const uint8_t note, const uint8_t velocity) {
    midi.noteOn(note, velocity, channel);
}

void tfw::hal::usb::RealHostComputer::sendMidiNoteOff(const uint8_t channel, const uint8_t note) {
    midi.noteOff(note, 0, channel);
}

void tfw::hal::usb::RealHostComputer::sendMidiControlChange(
    const uint8_t channel,
    const uint8_t controller,
    const uint8_t value
) {
    midi.controlChange(controller, value, channel);
}

#endif
