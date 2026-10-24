#include <Arduino.h>

#include <Preferences.h>
#include <SPI.h>
#include <Wire.h>

#include <tfw/hal/logging.h>
#include <tfw/hal/uart.h>
#include <tfw/hal/usb.h>
#include <tfw/hal/settings.h>
#include <tfw/hal/streams/OutputStream.h>
#include <tfw/hal/time/PlatformClock.h>

#include <memory>

#include "mkp/components/notifications/NotifierFactory.h"
#include "mkp/devices/common/DeviceConfigurationManager.h"
#include "mkp/firmwares/base/Firmware.h"
#include "mkp/firmwares/drive/DriveMode.h"
#include "mkp/firmwares/drive/DriveModeFirmware.h"

#include "esp_pm.h"

namespace {
    std::unique_ptr<mkp::firmwares::base::Firmware> firmware;
    std::unique_ptr<mkp::firmwares::drive::DriveModeFirmware> driveModeFirmware;
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
    std::unique_ptr<tfw::hal::streams::OutputStream> loggingOutput;
    std::unique_ptr<tfw::hal::time::PlatformClock> clock;
    std::unique_ptr<tfw::hal::fs::FileSystem> fileSystem;
    std::unique_ptr<tfw::hal::settings::PreferencesDeviceSettings> preferencesDeviceSettings;
    std::unique_ptr<tfw::hal::settings::CachedDeviceSettings> deviceSettings;

    std::unique_ptr<mkp::firmwares::base::ServiceLocator> serviceLocator;

    esp_err_t pmResult;

#if !ARDUINO_USB_CDC_ON_BOOT
    // Created by the setup rather than globally, as it registers with the USB stack when constructed, which must not
    // happen in the drive mode
    std::unique_ptr<USBCDC> usbSerial;
#define TheSerial (*usbSerial)
#else
#define TheSerial Serial
#endif

    void setupServiceLocator() {
        serialPort = tfw::hal::uart::SerialPort::from(TheSerial);
        preferences = std::make_unique<Preferences>();
        clock = std::make_unique<tfw::hal::time::PlatformClock>();
        deviceTypeDetector = std::make_unique<mkp::devices::common::DeviceTypeDetector>(*clock);
        deviceModeDetector = std::make_unique<mkp::firmwares::base::FirmwareModeDetector>();
        deviceConfigurationManager = std::make_unique<mkp::devices::common::DeviceConfigurationManager>(
            *preferences,
            *deviceTypeDetector
        );
        notifierFactory = std::make_unique<mkp::components::notifications::NotifierFactory>(TheSerial);
        i2cClient = std::make_unique<tfw::hal::i2c::Client>(Wire);
        i2cSlavePort = std::make_unique<tfw::hal::i2c::SlavePort>(Wire, *clock);
        usbConnection = tfw::hal::usb::Connection::create(*clock);
        metricRegistry = std::make_unique<tfw::hal::metrics::MetricRegistry>(*clock);
        fileSystem = std::make_unique<tfw::hal::fs::ArduinoFileSystem>();
        preferencesDeviceSettings = std::make_unique<tfw::hal::settings::PreferencesDeviceSettings>("settings");
        deviceSettings = std::make_unique<tfw::hal::settings::CachedDeviceSettings>(*preferencesDeviceSettings);

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
                .clock = *clock,
                .fileSystem = *fileSystem,
                .deviceSettings = *deviceSettings
            }
        );
    }

    void setupDriveMode() {
        // Logged to the UART, as the drive is the only USB device in the drive mode
        Serial.begin(115200);
        loggingOutput = std::unique_ptr<tfw::hal::streams::OutputStream>(
            tfw::hal::streams::createArduinoOutputStream(Serial)
        );
        tfw::hal::logging::initialize(loggingOutput.get());

        clock = std::make_unique<tfw::hal::time::PlatformClock>();

        driveModeFirmware = std::make_unique<mkp::firmwares::drive::DriveModeFirmware>(*clock);
        driveModeFirmware->setup();
    }
}

void setup() {
    esp_pm_config_t config{
        .max_freq_mhz = 80,
        .min_freq_mhz = 80,
        .light_sleep_enable = false,
    };

    pmResult = esp_pm_configure(&config);

    setCpuFrequencyMhz(80);

    if (mkp::firmwares::drive::consumeDriveModeRequest()) {
        setupDriveMode();
        return;
    }

#if !ARDUINO_USB_CDC_ON_BOOT
    usbSerial = std::make_unique<USBCDC>();
#endif

    // Room for a whole command line, as firmware updates send them back to back at close to the line length limit
    TheSerial.setRxBufferSize(2048);

    loggingOutput = std::unique_ptr<tfw::hal::streams::OutputStream>(
        tfw::hal::streams::createArduinoOutputStream(TheSerial)
    );
    tfw::hal::logging::initialize(loggingOutput.get());

    setupServiceLocator();

    fileSystem->begin();
    deviceSettings->begin();
    deviceModeDetector->setup();

    firmware = mkp::firmwares::base::Firmware::create(*serviceLocator);
    firmware->setup();
}

void loop() {
    if (driveModeFirmware) {
        driveModeFirmware->loop();
        return;
    }

    firmware->loop();
}
