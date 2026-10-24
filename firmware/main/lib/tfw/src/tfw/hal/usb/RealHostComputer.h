#pragma once

#include "HostComputer.h"

namespace tfw::hal::usb {
    // Sends input through the USB HID and MIDI devices of the ESP32 Arduino core
    class RealHostComputer final : public HostComputer {
    public:
        // Starts the HID devices, once USB has been started
        void begin();

        void pressKey(uint8_t keyCode) override;
        void releaseKey(uint8_t keyCode) override;
        void releaseAllKeys() override;
        void typeCharacter(char character) override;
        void pressConsumerControl(uint16_t usageId) override;
        void releaseConsumerControl() override;
        void pressSystemControl(uint8_t code) override;
        void releaseSystemControl() override;
        void clickMouse(uint8_t buttons) override;
        void moveMouse(int8_t x, int8_t y, int8_t wheel, int8_t pan) override;
        void sendMidiNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) override;
        void sendMidiNoteOff(uint8_t channel, uint8_t note) override;
        void sendMidiControlChange(uint8_t channel, uint8_t controller, uint8_t value) override;
    };
}
