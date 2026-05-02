#pragma once

#include "USBConnection.h"

namespace tfw::hal::usb {
    class NoOpConnection final : public Connection {
    public:
        void setup() override;
        bool isConnected() override;
        void sendAction(Action& action) override;
    };
}

