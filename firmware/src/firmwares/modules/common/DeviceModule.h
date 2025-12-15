#pragma once

#include <memory>
#include <firmwares/common/ServiceLocator.h>
#include <firmwares/common/runtimes/DeviceRuntime.h>

class DeviceModule {
public:
    virtual ~DeviceModule() = default;

    virtual void setup() = 0;
    virtual void loop() = 0;

    virtual RegisterManager& getRegisters() = 0;

    static std::unique_ptr<DeviceModule> local(
        DeviceConfiguration& config,
        ServiceLocator& serviceLocator
    );

    static std::unique_ptr<DeviceModule> remote(
        DeviceConfiguration& config,
        ServiceLocator& serviceLocator
    );

protected:
    DeviceModule() = default;
};
