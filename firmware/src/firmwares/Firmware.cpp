#include "Firmware.h"

#include "modules/FirmwareModuleA.h"

class NullFirmware final : public Firmware {
public:
    void setup() override {}
    void loop() override {}
    ~NullFirmware() override {}

};

std::unique_ptr<Firmware> Firmware::create(const char variant) {
    switch (variant) {
    case 'a':
    case 'A':
        return std::make_unique<FirmwareModuleA>();
    default:
        return std::make_unique<NullFirmware>();
    }
}