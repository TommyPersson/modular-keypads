#pragma once

#include <cstdio>
#include <string>
#include <vector>

#include <tfw/hal/usb/HostComputer.h>

namespace tfw::hal::usb::test {
    /**
     * Host computer for testing, that records the input it receives as readable events, such as "press key 0xe1".
     */
    class RecordingHostComputer final : public HostComputer {
    public:
        void pressKey(const uint8_t keyCode) override { record("press key 0x%02x", keyCode); }
        void releaseKey(const uint8_t keyCode) override { record("release key 0x%02x", keyCode); }
        void releaseAllKeys() override { record("release all keys"); }
        void typeCharacter(const char character) override { record("type '%c'", character); }
        void pressConsumerControl(const uint16_t usageId) override { record("press consumer control 0x%04x", usageId); }
        void releaseConsumerControl() override { record("release consumer control"); }
        void pressSystemControl(const uint8_t code) override { record("press system control 0x%02x", code); }
        void releaseSystemControl() override { record("release system control"); }
        void clickMouse(const uint8_t buttons) override { record("click mouse 0x%02x", buttons); }

        void moveMouse(const int8_t x, const int8_t y, const int8_t wheel, const int8_t pan) override {
            record("move mouse %d %d %d %d", x, y, wheel, pan);
        }

        // Returns the events recorded since the last call
        std::vector<std::string> takeEvents() {
            auto result = std::move(events);
            events.clear();
            return result;
        }

    private:
        template <typename... Args>
        void record(const char* format, Args... args) {
            char buffer[64];
            std::snprintf(buffer, sizeof(buffer), format, args...);
            events.emplace_back(buffer);
        }

        std::vector<std::string> events;
    };
}
