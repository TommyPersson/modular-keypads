#pragma once

#include <USB.h>
#include <USBHIDKeyboard.h>
#include <USBHIDConsumerControl.h>
#include <USBHIDSystemControl.h>

#include <Pins/OutputPin.h>

#include <SerialPort/SerialPort.h>

struct MacroPadHardware {
    OutputPin& indicatorLedPin;
    ESPUSB& usb;
    std::unique_ptr<SerialPort>& serial;
    USBHIDKeyboard& keyboard;
    USBHIDConsumerControl& consumerControl;
    USBHIDSystemControl& systemControl;
};

