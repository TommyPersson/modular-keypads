#include "LocalDeviceC.h"

#include <tfw/hal/logging.h>
#include <tfw/utils/strings.h>

#include "RegisterRefresherC.h"

namespace {
    auto logger = tfw::hal::logging::createLogger("LocalDeviceC");
}

using namespace mkp::devices::c;
using namespace mkp::devices::common;
using namespace mkp::components::notifications;
using namespace mkp::components::leds;
using namespace tfw::hal::spi;
using namespace tfw::hal::gpio;

LocalDeviceC::LocalDeviceC(
    const DeviceConfiguration& configuration,
    const DeviceConfigurationManager& configurationManager,
    const NotifierFactory& notifierFactory,
    tfw::hal::time::Clock& clock
) : LocalDevice(configuration, configurationManager, notifierFactory, IndicatorLedManager::NoOp(0, clock), clock) {
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

    subDevice1 = std::make_unique<SubDevice>(1, *spi, *pinModCs1, *pinMod1Int);
    subDevice2 = std::make_unique<SubDevice>(2, *spi, *pinModCs2, *pinMod2Int);
    subDevice3 = std::make_unique<SubDevice>(3, *spi, *pinModCs3, *pinMod3Int);

    subDevice1->setup();
    subDevice2->setup();
    subDevice3->setup();

    subDevice1->onSwitchEvent().addObserver(this);
    subDevice2->onSwitchEvent().addObserver(this);
    subDevice3->onSwitchEvent().addObserver(this);

    pinModReset = OutputPin::physical(4);
    pinModReset->init();
    pinModReset->setHigh();

    registerRefresher->setup();
}

void LocalDeviceC::loop() {
    LocalDevice::loop();

    subDevice1->update();
    subDevice2->update();
    subDevice3->update();

    registerRefresher->loop();
}
