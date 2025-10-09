#pragma once

#include <firmwares/Firmware.h>
#include <firmwares/common/commands/CommandProcessor.h>
#include <Registers/Register.h>
#include <SerialPort/SerialPort.h>

class GenericFirmware final : public Firmware {
public:
    explicit GenericFirmware(
        DeviceConfigurationManager& deviceConfigurationManager,
        SerialPort& serialPort,
        Logger& logger
    );

    ~GenericFirmware() override;

    void setup() override;
    void loop() override;

private:
    std::vector<std::shared_ptr<Register>> registers;
};
