#include "LocalDeviceC.h"

#include <tfw/hal/logging.h>
#include <tfw/utils/strings.h>

#include "RegisterRefresherC.h"

namespace {
    auto logger = tfw::hal::logging::createLogger("LocalDeviceC");
}

using namespace mkp::devices::c;
using namespace mkp::devices::common;
using namespace mkp::components::leds;
using namespace tfw::hal::spi;
using namespace tfw::hal::gpio;

LocalDeviceC::LocalDeviceC(
    const DeviceConfiguration& configuration,
    const DeviceConfigurationManager& configurationManager,
    tfw::hal::time::Clock& clock
) : LocalDevice(configuration, configurationManager, IndicatorLedManager::NoOp(0, clock), clock) {
    registerRefresher = std::make_unique<RegisterRefresherC>(*registerManager, clock);
}

LocalDeviceC::~LocalDeviceC() = default;

void LocalDeviceC::setup() {
    LocalDevice::setup();

    pinModMiso = InputPin::physical(39);
    pinModMosi = OutputPin::physical(40);
    pinModSck = OutputPin::physical(41);

    spi = std::make_unique<SPIClass>(FSPI);
    spi->begin(pinModSck->pinNumber, pinModMiso->pinNumber, pinModMosi->pinNumber);

    pinModCs1 = OutputPin::physical(5);
    pinModCs2 = OutputPin::physical(6);
    pinModCs3 = OutputPin::physical(7);

    pinMod1Int = InputPin::physical(12, PULLUP);
    pinMod2Int = InputPin::physical(13, PULLUP);
    pinMod3Int = InputPin::physical(14, PULLUP);

    subDevice1 = std::make_unique<SubDevice>(clock, configuration.address, configuration.id, 1, *spi, *pinModCs1, *pinMod1Int);
    subDevice2 = std::make_unique<SubDevice>(clock, configuration.address, configuration.id, 2, *spi, *pinModCs2, *pinMod2Int);
    subDevice3 = std::make_unique<SubDevice>(clock, configuration.address, configuration.id, 3, *spi, *pinModCs3, *pinMod3Int);

    subDevices[0] = subDevice1.get();
    subDevices[1] = subDevice2.get();
    subDevices[2] = subDevice3.get();

    subDevice1->setup();
    subDevice2->setup();
    subDevice3->setup();

    subDevice1->onSwitchEvent().addObserver(this);
    subDevice1->onRotaryEncoderEvent().addObserver(this);
    subDevice1->onConnected().addObserver(this);
    subDevice1->onDisconnected().addObserver(this);
    subDevice1->onRenamed().addObserver(this);

    subDevice2->onSwitchEvent().addObserver(this);
    subDevice2->onRotaryEncoderEvent().addObserver(this);
    subDevice2->onConnected().addObserver(this);
    subDevice2->onDisconnected().addObserver(this);
    subDevice2->onRenamed().addObserver(this);

    subDevice3->onSwitchEvent().addObserver(this);
    subDevice3->onRotaryEncoderEvent().addObserver(this);
    subDevice3->onConnected().addObserver(this);
    subDevice3->onDisconnected().addObserver(this);
    subDevice3->onRenamed().addObserver(this);

    pinModReset = OutputPin::physical(4);
    pinModReset->init();
    pinModReset->setHigh();

    registerRefresher->setup();
}

void LocalDeviceC::loop() {
    LocalDevice::loop();

    subDevice1->loop();
    subDevice2->loop();
    subDevice3->loop();

    registerRefresher->loop();
}

std::span<Device* const> LocalDeviceC::getSubDevices() const {
    return subDevices;
}

tfw::utils::void_result LocalDeviceC::flashIdentificationLights(const uint32_t durationMs) {
    // Sub-devices can be disconnected at any time, so their failures are ignored
    for (const auto subDevice : subDevices) {
        if (subDevice->getConfiguration().id != 0) {
            subDevice->flashIdentificationLights(durationMs);
        }
    }

    return LocalDevice::flashIdentificationLights(durationMs);
}

void LocalDeviceC::observe(const SubDeviceConnectedEvent& event) {
    subDeviceConnectedSubject.notify({ .deviceId = event.subDeviceId });
}

void LocalDeviceC::observe(const SubDeviceDisconnectedEvent& event) {
    subDeviceDisconnectedSubject.notify({ .deviceId = event.subDeviceId });
}

void LocalDeviceC::observe(const SubDeviceRenamedEvent& event) {
    subDeviceRenamedSubject.notify({ .deviceId = event.subDeviceId });
}
