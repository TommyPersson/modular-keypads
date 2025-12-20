#pragma once

#include <memory>
#include "USBAction.h"

namespace usb {
    class Connection {
    public:
        virtual ~Connection() = default;

        virtual void setup() = 0;
        virtual bool isConnected() = 0;

        virtual void sendAction(Action& action) = 0;

        static std::unique_ptr<Connection> create();
    };
}