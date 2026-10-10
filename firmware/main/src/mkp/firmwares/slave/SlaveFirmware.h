#pragma once

#include "mkp/firmwares/base/Firmware.h"

class SlaveFirmware final
    : public mkp::firmwares::base::Firmware,
      public tfw::utils::observables::Observer<mkp::devices::common::DeviceSwitchEvent>,
      public tfw::utils::observables::Observer<mkp::devices::common::DeviceRotaryEncoderEvent>,
      public tfw::utils::observables::Observer<mkp::devices::common::DeviceConnectedEvent>,
      public tfw::utils::observables::Observer<mkp::devices::common::DeviceDisconnectedEvent>,
      public tfw::utils::observables::Observer<mkp::devices::common::DeviceRenamedEvent> {
public:
    explicit SlaveFirmware(mkp::firmwares::base::ServiceLocator& serviceLocator);
    ~SlaveFirmware() override;

    void setup() override;
    void loop() override;

    void observe(const mkp::devices::common::DeviceSwitchEvent& event) override;
    void observe(const mkp::devices::common::DeviceRotaryEncoderEvent& event) override;
    void observe(const mkp::devices::common::DeviceConnectedEvent& event) override;
    void observe(const mkp::devices::common::DeviceDisconnectedEvent& event) override;
    void observe(const mkp::devices::common::DeviceRenamedEvent& event) override;

private:
    void updateDeviceNameEndpoint();
    void updateSubDeviceEndpoints();

    std::unique_ptr<mkp::devices::common::LocalDevice> device;
    std::vector<mkp::devices::common::i2c::structs::SubDeviceInformation> subDeviceInformation;
    tfw::hal::i2c::SlavePort& slavePort;
};
