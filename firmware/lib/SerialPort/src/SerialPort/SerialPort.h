#pragma once

#include <HardwareSerial.h>
#include <memory>

#if SOC_USB_OTG_SUPPORTED
#include <USBCDC.h>
#endif

#include <Stream.h>

namespace utils::serial {
    class SerialPort {
    public:
        virtual ~SerialPort() = default;
        virtual void begin(unsigned long baud) = 0;
        virtual void end() = 0;
        virtual Stream& stream() = 0;

        static std::unique_ptr<SerialPort> from(HardwareSerial& serial);

#if SOC_USB_OTG_SUPPORTED
        static std::unique_ptr<SerialPort> from(USBCDC& serial);
#endif

        static std::unique_ptr<SerialPort> from(HWCDC& serial);

    };
}