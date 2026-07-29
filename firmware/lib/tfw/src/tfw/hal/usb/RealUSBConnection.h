#pragma once

#include "USBConnection.h"
#include "tfw/utils/ring_queue.h"

namespace tfw::hal::usb {
    class RealConnection final : public Connection {
    public:
        void setup() override;
        void update() override;
        bool isConnected() override;
        void sendAction(Action& action) override;
    private:
        utils::ring_queue<char> characterOutputQueue{1000}; // enforce size limit in storage and UI also
        uint64_t lastKeyboardWriteTime = 0;
    };
}
