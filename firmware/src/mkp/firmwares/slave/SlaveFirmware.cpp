#include "SlaveFirmware.h"

#include <tfw/hal/logging.h>
#include <tfw/utils/strings.h>

#include "mkp/components/events/RemoteEventTypes.h"

#include "i2c/commands/FlashButtonIdentificationLightRemoteCommandHandler.h"
#include "i2c/commands/FlashDeviceIdentificationLightsRemoteCommandHandler.h"
#include "i2c/commands/RenameDeviceRemoteCommandHandler.h"

namespace {
    auto logger = tfw::hal::logging::createLogger("SlaveFirmware");
}

using namespace mkp::firmwares::base;
using namespace firmwares::slave::i2c::commands;

SlaveFirmware::SlaveFirmware(ServiceLocator& serviceLocator) :
    Firmware(serviceLocator),
    slavePort(serviceLocator.i2cSlavePort) {
}

SlaveFirmware::~SlaveFirmware() = default;

void SlaveFirmware::setup() {
    Firmware::setup();

    auto configuration = serviceLocator.deviceConfigurationManager.getDeviceConfiguration();

    const auto deviceFactory = getDeviceFactory(configuration.type);
    if (deviceFactory == nullptr) {
        logger->error("No device factory found for type: %c", configuration.type);
        return;
    }

    device = deviceFactory->createLocal(configuration, serviceLocator);
    device->setup();

    registers = &device->getRegisters();

    const auto deviceAddress = configuration.address;
    if (deviceAddress == 0) {
        logger->error("Device needs to have an address configured.");
        return;
    }

    const auto deviceId = configuration.id;
    const auto deviceName = configuration.name;

    mkp::devices::common::i2c::structs::DeviceInformation deviceInformationStruct;
    deviceInformationStruct.deviceId = deviceId;
    deviceInformationStruct.deviceType = configuration.type;

    slavePort.updateEndpoint(
        mkp::devices::common::i2c::endpoints::DeviceInformation,
        &deviceInformationStruct
    );

    slavePort.updateEndpoint(
        mkp::devices::common::i2c::endpoints::DeviceName,
        deviceName.c_str()
    );

    slavePort.setup(deviceAddress, device->getI2cPins(), device->getEventInterruptPin());

    slavePort.addCommandHandler(new FlashDeviceIdentificationLightsRemoteCommandHandler(*device));
    slavePort.addCommandHandler(new FlashButtonIdentificationLightRemoteCommandHandler(*device));
    slavePort.addCommandHandler(new RenameDeviceRemoteCommandHandler(*device));

    device->onSwitchEvent().addObserver(this);
}

void SlaveFirmware::loop() {
    Firmware::loop();

    device->loop();

    mkp::devices::common::i2c::structs::DeviceRegisters registersStruct;
    auto registerData = device->getRegisters().readAll();
    std::memcpy(&registersStruct.data, registerData.data(), sizeof(registersStruct.data));

    slavePort.updateEndpoint(
        mkp::devices::common::i2c::endpoints::DeviceRegisters,
        &registersStruct,
        device->getRegisterDescriptors().size()
    );
    /*
        gpio_wakeup_enable((gpio_num_t)11, GPIO_INTR_LOW_LEVEL);
        gpio_wakeup_enable((gpio_num_t)10, GPIO_INTR_LOW_LEVEL);
        esp_sleep_enable_gpio_wakeup();
        esp_sleep_enable_timer_wakeup(1000);
        esp_light_sleep_start();
        */
}

void SlaveFirmware::observe(const mkp::devices::common::DeviceSwitchEvent& event) {
    if (event.state == tfw::hal::buttons::ButtonState::PRESSED) {
        slavePort.enqueueEvent(
            tfw::hal::i2c::Event{
                .type = static_cast<uint8_t>(mkp::components::events::RemoteEventType::BUTTON_PRESSED),
                .deviceId = event.deviceId,
                .args = {event.switchNumber}
            }
        );
    } else if (event.state == tfw::hal::buttons::ButtonState::UNPRESSED) {
        slavePort.enqueueEvent(
            tfw::hal::i2c::Event{
                .type = static_cast<uint8_t>(mkp::components::events::RemoteEventType::BUTTON_RELEASED),
                .deviceId = event.deviceId,
                .args = {event.switchNumber}
            }
        );
    }

    slavePort.triggerEventInterrupt();
}
