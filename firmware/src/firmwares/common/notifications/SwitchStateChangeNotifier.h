#pragma once

#include <tfw/hal/buttons/Button.h>
#include <tfw/utils/observables.h>
#include "firmwares/common/runtimes/DeviceRuntime.h"

#include "Notifier.h"

class SwitchStateChangeNotifier
    : public tfw::utils::observables::Observer<tfw::hal::buttons::ButtonStateChangedEvent>,
      public tfw::utils::observables::Observer<devices::DeviceSwitchEvent> {
public:
    explicit SwitchStateChangeNotifier(Notifier& notifier);
    ~SwitchStateChangeNotifier() override;

    void observe(const tfw::hal::buttons::ButtonStateChangedEvent& event) override;
    void observe(const devices::DeviceSwitchEvent& event) override;

private:
    Notifier& notifier;
};
