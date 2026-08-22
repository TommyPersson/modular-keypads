#include "LocalDeviceA.h"

#include <tfw/hal/logging.h>
#include <tfw/utils/strings.h>

#include "RegisterRefresherA.h"

namespace {
    auto logger = tfw::hal::logging::createLogger("LocalDeviceA");
}

using namespace mkp::devices::a;
using namespace mkp::devices::common;
using namespace mkp::components::notifications;
using namespace mkp::components::leds;

LocalDeviceA::LocalDeviceA(
    const DeviceConfiguration& configuration,
    const DeviceConfigurationManager& configurationManager,
    const NotifierFactory& notifierFactory,
    tfw::hal::time::Clock& clock
) : LocalDevice(configuration, configurationManager, notifierFactory, IndicatorLedManager::NeoPixel(12, 48, clock), clock) {
    registerRefresher = std::make_unique<RegisterRefresherA>(*registerManager, clock);
}

LocalDeviceA::~LocalDeviceA() = default;

void LocalDeviceA::setup() {
    LocalDevice::setup();

    registerRefresher->setup();
}

void LocalDeviceA::loop() {
    LocalDevice::loop();

    registerRefresher->loop();
}
