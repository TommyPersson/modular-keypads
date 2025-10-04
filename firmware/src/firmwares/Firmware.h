#pragma once

#include <memory>

#include "common/DeviceConfigurationManager.h"

class Firmware {
public:
    virtual ~Firmware() = default;
    virtual void setup() = 0;
    virtual void loop() = 0;

    static std::unique_ptr<Firmware> create(
        char variant,
        DeviceConfigurationManager& deviceConfigurationManager,
        Logger& logger
    );
};
