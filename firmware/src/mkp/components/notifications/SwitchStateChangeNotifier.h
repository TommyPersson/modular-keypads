#pragma once

#include <tfw/hal/buttons/Button.h>
#include <tfw/utils/observables.h>

#include "Notifier.h"
#include "mkp/devices/common/Device.h"

namespace mkp::components::notifications {
    class SwitchStateChangeNotifier
        : public tfw::utils::observables::Observer<tfw::hal::buttons::ButtonStateChangedEvent>,
          public tfw::utils::observables::Observer<mkp::devices::common::DeviceSwitchEvent> {
    public:
        explicit SwitchStateChangeNotifier(Notifier& notifier);
        ~SwitchStateChangeNotifier() override;

        void observe(const tfw::hal::buttons::ButtonStateChangedEvent& event) override;
        void observe(const mkp::devices::common::DeviceSwitchEvent& event) override;

    private:
        Notifier& notifier;
    };
}