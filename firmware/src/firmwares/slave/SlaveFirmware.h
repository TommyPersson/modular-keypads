#pragma once

#include "firmwares/base/Firmware.h"

class SlaveFirmware final
    : public Firmware,
      tfw::utils::observables::Observer<devices::DeviceSwitchEvent> {
public:
    explicit SlaveFirmware(ServiceLocator& serviceLocator);
    ~SlaveFirmware() override;

    void setup() override;
    void loop() override;

    void observe(const devices::DeviceSwitchEvent& event) override;

private:

    std::unique_ptr<devices::LocalDevice> device;

    tfw::hal::i2c::SlavePort& slavePort;
};
