#include "RealHostComputer.h"

#include "USB.h"

#ifdef SOC_USB_OTG_SUPPORTED

#include <USBHIDConsumerControl.h>
#include <USBHIDKeyboard.h>
#include <USBHIDMouse.h>
#include <USBHIDSystemControl.h>
#include <USBMIDI.h>

// Created with the host computer rather than globally, as the devices register with the USB stack when constructed,
// which must only happen in firmwares that use them, and before USB is started
struct tfw::hal::usb::RealHostComputer::Devices {
    USBHIDKeyboard keyboard;
    USBHIDConsumerControl consumerControl;
    USBHIDSystemControl systemControl;
    USBHIDMouse mouse;
    USBMIDI midi;
};

tfw::hal::usb::RealHostComputer::RealHostComputer()
    : devices(std::make_unique<Devices>()) {
}

tfw::hal::usb::RealHostComputer::~RealHostComputer() = default;

void tfw::hal::usb::RealHostComputer::begin() {
    devices->keyboard.begin();
    devices->consumerControl.begin();
    devices->systemControl.begin();
    devices->mouse.begin();
    devices->midi.begin();
}

void tfw::hal::usb::RealHostComputer::pressKey(const uint8_t keyCode) {
    devices->keyboard.pressRaw(keyCode);
}

void tfw::hal::usb::RealHostComputer::releaseKey(const uint8_t keyCode) {
    devices->keyboard.releaseRaw(keyCode);
}

void tfw::hal::usb::RealHostComputer::releaseAllKeys() {
    devices->keyboard.releaseAll();
}

void tfw::hal::usb::RealHostComputer::typeCharacter(const char character) {
    devices->keyboard.write(character);
}

void tfw::hal::usb::RealHostComputer::pressConsumerControl(const uint16_t usageId) {
    devices->consumerControl.press(usageId);
}

void tfw::hal::usb::RealHostComputer::releaseConsumerControl() {
    devices->consumerControl.release();
}

void tfw::hal::usb::RealHostComputer::pressSystemControl(const uint8_t code) {
    devices->systemControl.press(code);
}

void tfw::hal::usb::RealHostComputer::releaseSystemControl() {
    devices->systemControl.release();
}

void tfw::hal::usb::RealHostComputer::clickMouse(const uint8_t buttons) {
    devices->mouse.click(buttons);
}

void tfw::hal::usb::RealHostComputer::moveMouse(const int8_t x, const int8_t y, const int8_t wheel, const int8_t pan) {
    devices->mouse.move(x, y, wheel, pan);
}

void tfw::hal::usb::RealHostComputer::sendMidiNoteOn(const uint8_t channel, const uint8_t note, const uint8_t velocity) {
    devices->midi.noteOn(note, velocity, channel);
}

void tfw::hal::usb::RealHostComputer::sendMidiNoteOff(const uint8_t channel, const uint8_t note) {
    devices->midi.noteOff(note, 0, channel);
}

void tfw::hal::usb::RealHostComputer::sendMidiControlChange(
    const uint8_t channel,
    const uint8_t controller,
    const uint8_t value
) {
    devices->midi.controlChange(controller, value, channel);
}

#endif
