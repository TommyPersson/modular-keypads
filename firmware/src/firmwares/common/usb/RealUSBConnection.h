#pragma once

#include "USBConnection.h"

namespace usb {
    class RealConnection final : public Connection {
    public:
        void setup() override;
        bool isConnected() override;
        void sendAction(Action& action) override;

    private:
    };
}
