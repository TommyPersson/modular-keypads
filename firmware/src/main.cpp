#include <Arduino.h>

#include <Preferences.h>
#include <SPI.h>
#include <Wire.h>

#include <tfw/hal/logging.h>
#include <tfw/hal/uart.h>
#include <tfw/hal/usb.h>

#include <memory>

#include "mkp/components/notifications/NotifierFactory.h"
#include "mkp/devices/common/DeviceConfigurationManager.h"
#include "mkp/firmwares/base/Firmware.h"

#include "esp_pm.h"

namespace {
    std::unique_ptr<mkp::firmwares::base::Firmware> firmware;
    std::unique_ptr<Preferences> preferences;
    std::unique_ptr<tfw::hal::uart::SerialPort> serialPort;
    std::unique_ptr<mkp::components::notifications::NotifierFactory> notifierFactory;
    std::unique_ptr<mkp::devices::common::DeviceConfigurationManager> deviceConfigurationManager;
    std::unique_ptr<tfw::hal::i2c::Client> i2cClient;
    std::unique_ptr<tfw::hal::i2c::SlavePort> i2cSlavePort;
    std::unique_ptr<tfw::hal::usb::Connection> usbConnection;
    std::unique_ptr<tfw::hal::metrics::MetricRegistry> metricRegistry;
    std::unique_ptr<mkp::devices::common::DeviceTypeDetector> deviceTypeDetector;
    std::unique_ptr<mkp::firmwares::base::FirmwareModeDetector> deviceModeDetector;

    std::unique_ptr<mkp::firmwares::base::ServiceLocator> serviceLocator;

    esp_err_t pmResult;

#if !ARDUINO_USB_CDC_ON_BOOT
    USBCDC TheSerial;
#else
#define TheSerial Serial
#endif

    void setupServiceLocator() {
        serialPort = tfw::hal::uart::SerialPort::from(TheSerial);
        preferences = std::make_unique<Preferences>();
        deviceTypeDetector = std::make_unique<mkp::devices::common::DeviceTypeDetector>();
        deviceModeDetector = std::make_unique<mkp::firmwares::base::FirmwareModeDetector>();
        deviceConfigurationManager = std::make_unique<mkp::devices::common::DeviceConfigurationManager>(
            *preferences,
            *deviceTypeDetector
        );
        notifierFactory = std::make_unique<mkp::components::notifications::NotifierFactory>(TheSerial);
        i2cClient = std::make_unique<tfw::hal::i2c::Client>(Wire);
        i2cSlavePort = std::make_unique<tfw::hal::i2c::SlavePort>(Wire);
        usbConnection = tfw::hal::usb::Connection::create();
        metricRegistry = std::make_unique<tfw::hal::metrics::MetricRegistry>();

        serviceLocator = std::make_unique<mkp::firmwares::base::ServiceLocator>(
            mkp::firmwares::base::ServiceLocator{
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
    }
}

void setup() {
    esp_pm_config_t config{
        .max_freq_mhz = 80,
        .min_freq_mhz = 80,
    };

    pmResult = esp_pm_configure(&config);

    setCpuFrequencyMhz(80);

    tfw::hal::logging::initialize(&TheSerial);

    setupServiceLocator();

    deviceModeDetector->setup();

    firmware = mkp::firmwares::base::Firmware::create(*serviceLocator);
    firmware->setup();
}

void loop() {
    firmware->loop();
}
