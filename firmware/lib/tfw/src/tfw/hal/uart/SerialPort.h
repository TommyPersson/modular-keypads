#pragma once

#include <memory>

#ifdef ARDUINO
#include <HardwareSerial.h>

#if SOC_USB_OTG_SUPPORTED
#include <USBCDC.h>
#endif

#include <Stream.h>
#endif // ARDUINO

namespace tfw::hal::uart {
    class SerialPort {
    public:
        virtual ~SerialPort() = default;
        virtual void begin(unsigned long baud) = 0;
        virtual void end() = 0;

#ifdef ARDUINO
        virtual Stream& stream() = 0;

        static std::unique_ptr<SerialPort> from(HardwareSerial& serial);

#if SOC_USB_OTG_SUPPORTED
        static std::unique_ptr<SerialPort> from(USBCDC& serial);
#endif

        static std::unique_ptr<SerialPort> from(HWCDC& serial);
#endif // ARDUINO

    };
}