#pragma once

namespace mkp::components::events {
    enum class RemoteEventType : uint8_t {
        BUTTON_PRESSED = 0x10,
        BUTTON_RELEASED = 0x11,
        BUTTON_LONG_PRESSED = 0x12,
        ROTARY_ENCODER_CW = 0x20,
        ROTARY_ENCODER_CCW = 0x21,
        SUB_DEVICE_CONNECTED = 0x30,
        SUB_DEVICE_DISCONNECTED = 0x31,
        SUB_DEVICE_RENAMED = 0x32,
        DEVICE_RENAMED = 0x40,
    };
}
