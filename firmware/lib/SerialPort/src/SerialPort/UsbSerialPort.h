#pragma once

#include <USBCDC.h>
#include "SerialPort.h"

class UsbSerialPort : public SerialPort {
public:
    UsbSerialPort(USBCDC& serial) : serial(serial) {}
    void begin(unsigned long baud) override { serial.begin(baud); }
    void end() override { serial.end(); }
    Stream& stream() override { return serial; }

private:
    USBCDC& serial;
};
