#pragma once

#include <HardwareSerial.h>

#include "SerialPort.h"

class HardwareSerialPort : public SerialPort {
public:
    HardwareSerialPort(HardwareSerial& serial) : serial(serial) {}
    void begin(unsigned long baud) override { serial.begin(baud); }
    void end() override { serial.end(); }
    Stream& stream() override { return serial; }

private:
    HardwareSerial& serial;
};
