#include "LocalDeviceA.h"

#include <firmwares/slave/i2c/commands/FlashDeviceIdentificationLightsRemoteCommandHandler.h>

#include <tfw/hal/logging.h>

#include "firmwares/modules/a/LocalRegisterRefresherA.h"
#include "tfw/utils/strings.h"

namespace {
    auto logger = tfw::hal::logging::createLogger("LocalDeviceA");
}

using namespace devices::a;

LocalDeviceA::LocalDeviceA(
    const DeviceConfiguration& configuration,
    const DeviceConfigurationManager& configurationManager,
    const NotifierFactory& notifierFactory
) : LocalDevice(configuration, configurationManager, notifierFactory, IndicatorLedManager::NeoPixel(12, 48)) {
    registerRefresher = std::make_unique<LocalRegisterRefresherA>(*registerManager);
}

LocalDeviceA::~LocalDeviceA() = default;

void LocalDeviceA::setup() {
    registerRefresher->setup();

    LocalDevice::setup();
}

void LocalDeviceA::loop() {
    LocalDevice::loop();

    registerRefresher->loop();
}
