#include "Firmware.h"

#include "modules/a/FirmwareModuleA.h"

class NullFirmware final : public Firmware {
public:
    void setup() override {
    }

    void loop() override {
    }

    ~NullFirmware() override = default;
};

std::unique_ptr<Firmware> Firmware::create(
    const char variant,
    DeviceConfigurationManager& deviceConfigurationManager,
    Logger& logger
    ) {

    switch (variant) {
    case 'a':
    case 'A':
        return std::make_unique<FirmwareModuleA>(deviceConfigurationManager, logger);
    default:
        return std::make_unique<NullFirmware>();
    }
}
