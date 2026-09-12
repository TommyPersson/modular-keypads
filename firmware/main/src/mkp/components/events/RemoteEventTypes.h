#pragma once

namespace mkp::components::events {
    enum class RemoteEventType : uint8_t {
        BUTTON_PRESSED = 0x10,
        BUTTON_RELEASED = 0x11,
        ROTARY_ENCODER_CW = 0x20,
        ROTARY_ENCODER_CCW = 0x21,
    };
}
