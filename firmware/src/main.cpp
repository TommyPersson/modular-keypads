#include <Arduino.h>

#include <Preferences.h>
#include <SPI.h>
#include <Wire.h>

#include <tfw/hal/uart.h>

#include <memory>

#include "firmwares/base/Firmware.h"
#include "firmwares/common/DeviceConfigurationManager.h"
#include "firmwares/common/ServiceLocator.h"
#include "firmwares/common/notifications/NotifierFactory.h"
#include <tfw/hal/logging.h>
#include <tfw/hal/usb.h>

#include "esp_pm.h"

std::unique_ptr<Firmware> firmware;
std::unique_ptr<Preferences> preferences;
std::unique_ptr<tfw::utils::serial::SerialPort> serialPort;
std::unique_ptr<NotifierFactory> notifierFactory;
std::unique_ptr<DeviceConfigurationManager> deviceConfigurationManager;
std::unique_ptr<tfw::utils::i2c::Client> i2cClient;
std::unique_ptr<tfw::utils::i2c::SlavePort> i2cSlavePort;
std::unique_ptr<tfw::utils::usb::Connection> usbConnection;
std::unique_ptr<tfw::utils::metrics::MetricRegistry> metricRegistry;
std::unique_ptr<devices::DeviceTypeDetector> deviceTypeDetector;
std::unique_ptr<devices::DeviceModeDetector> deviceModeDetector;

std::unique_ptr<ServiceLocator> serviceLocator;


namespace {
    esp_err_t pmResult;
}

#if !ARDUINO_USB_CDC_ON_BOOT
USBCDC TheSerial;
#else
#define TheSerial Serial
#endif



void setup() {
    esp_pm_config_t config{
        .max_freq_mhz = 80,
        .min_freq_mhz = 80,
    };

    pmResult = esp_pm_configure(&config);

    setCpuFrequencyMhz(80);

    tfw::utils::logging::initialize(&TheSerial);

    serialPort = tfw::utils::serial::SerialPort::from(TheSerial);
    preferences = std::make_unique<Preferences>();
    deviceTypeDetector = std::make_unique<devices::DeviceTypeDetector>();
    deviceModeDetector = std::make_unique<devices::DeviceModeDetector>();
    deviceConfigurationManager = std::make_unique<DeviceConfigurationManager>(*preferences, *deviceTypeDetector);
    notifierFactory = std::make_unique<NotifierFactory>(TheSerial);
    i2cClient = std::make_unique<tfw::utils::i2c::Client>(Wire);
    i2cSlavePort = std::make_unique<tfw::utils::i2c::SlavePort>(Wire);
    usbConnection = tfw::utils::usb::Connection::create();
    metricRegistry = std::make_unique<tfw::utils::metrics::MetricRegistry>();

    serviceLocator = std::make_unique<ServiceLocator>(
        ServiceLocator{
            .deviceConfigurationManager = *deviceConfigurationManager,
            .serialPort = *serialPort,
            .notifierFactory = *notifierFactory,
            .i2cClient = *i2cClient,
            .i2cSlavePort = *i2cSlavePort,
            .usbConnection = *usbConnection,
            .metricRegistry = *metricRegistry,
            .deviceModeDetector = *deviceModeDetector,
        }
    );

    deviceModeDetector->setup();

    firmware = Firmware::create(*serviceLocator);
    firmware->setup();
}

void loop() {
    firmware->loop();
}
