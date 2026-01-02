#pragma once

#include "SerialPort.h"

namespace utils::serial {
    class HWCDCSerialPort : public SerialPort {
    public:
        HWCDCSerialPort(HWCDC& serial) : serial(serial) {}
        void begin(unsigned long baud) override { serial.begin(baud); }
        void end() override { serial.end(); }
        Stream& stream() override { return serial; }

    private:
        HWCDC& serial;
    };
}