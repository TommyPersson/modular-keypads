#pragma once

#include "mkp/firmwares/base/Firmware.h"

class SlaveFirmware final
    : public mkp::firmwares::base::Firmware,
      tfw::utils::observables::Observer<mkp::devices::common::DeviceSwitchEvent> {
public:
    explicit SlaveFirmware(mkp::firmwares::base::ServiceLocator& serviceLocator);
    ~SlaveFirmware() override;

    void setup() override;
    void loop() override;

    void observe(const mkp::devices::common::DeviceSwitchEvent& event) override;

private:
    std::unique_ptr<mkp::devices::common::LocalDevice> device;
    tfw::hal::i2c::SlavePort& slavePort;
};
