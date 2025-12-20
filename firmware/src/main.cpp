#include <Arduino.h>

#include <SPI.h>
#include <Preferences.h>
#include <Wire.h>

#include <SerialPort/SerialPort.h>

#include <memory>

#include "firmwares/Firmware.h"
#include "firmwares/common/DeviceConfigurationManager.h"
#include "firmwares/common/ServiceLocator.h"
#include "firmwares/common/logging/Logger.h"
#include "firmwares/common/notifications/NotifierFactory.h"
#include "firmwares/common/usb/USB.h"


std::unique_ptr<Firmware> firmware;
std::unique_ptr<Preferences> preferences;
std::unique_ptr<SerialPort> serialPort;
std::unique_ptr<NotifierFactory> notifierFactory;
std::unique_ptr<DeviceConfigurationManager> deviceConfigurationManager;
std::unique_ptr<I2cClient> i2cClient;
std::unique_ptr<i2c::SlavePort> i2cSlavePort;
std::unique_ptr<usb::Connection> usbConnection;

std::unique_ptr<ServiceLocator> serviceLocator;


#if !ARDUINO_USB_CDC_ON_BOOT
USBCDC TheSerial;
#else
#define TheSerial Serial
#endif

void setup() {
    common::logging::initialize(&TheSerial);


    serialPort = SerialPort::from(TheSerial);
    preferences = std::make_unique<Preferences>();
    deviceConfigurationManager = std::make_unique<DeviceConfigurationManager>(*preferences);
    notifierFactory = std::make_unique<NotifierFactory>(TheSerial);
    i2cClient = std::make_unique<I2cClient>(Wire);
    i2cSlavePort = std::make_unique<i2c::SlavePort>(Wire);
    usbConnection = usb::Connection::create();

    serviceLocator = std::make_unique<ServiceLocator>(ServiceLocator{
        .deviceConfigurationManager = *deviceConfigurationManager,
        .serialPort = *serialPort,
        .notifierFactory = *notifierFactory,
        .i2cClient = *i2cClient,
        .i2cSlavePort = *i2cSlavePort,
        .i2c = Wire,
        .usbConnection = *usbConnection,
    });

    firmware = Firmware::create(*serviceLocator);
    firmware->setup();
}

void loop() {
    firmware->loop();
}
