#pragma once

#include <HardwareSerial.h>
#include <memory>
#include <USBCDC.h>
#include <Stream.h>

class SerialPort {
public:
    virtual ~SerialPort() = default;
    virtual void begin(unsigned long baud) = 0;
    virtual void end() = 0;
    virtual Stream& stream() = 0;

    static std::unique_ptr<SerialPort> from(HardwareSerial& serial);
    static std::unique_ptr<SerialPort> from(USBCDC& serial);
};