#pragma once

#include <memory>
#include <firmwares/common/ServiceLocator.h>
#include <firmwares/common/runtimes/DeviceRuntime.h>
#include <firmwares/common/runtimes/RegisterRefresher.h>

class DeviceModule {
public:
    DeviceModule(
        ServiceLocator& serviceLocator,
        std::unique_ptr<DeviceRuntime> deviceRuntime,
        std::unique_ptr<RegisterRefresher> registerRefresher
    );

    virtual ~DeviceModule();

    virtual void setup();
    virtual void loop();

    static std::unique_ptr<DeviceModule> create(char type, DeviceMode mode);

private:
    std::unique_ptr<DeviceRuntime> deviceRuntime;
    std::unique_ptr<RegisterRefresher> registerRefresher;
};
