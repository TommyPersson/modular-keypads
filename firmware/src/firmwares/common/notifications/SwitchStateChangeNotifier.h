#pragma once

#include <tfw/hal/buttons/Button.h>
#include <tfw/utils/observables.h>

#include "Notifier.h"

class SwitchStateChangeNotifier : public tfw::utils::observables::Observer<tfw::hal::buttons::ButtonStateChangedEvent> {
public:
    explicit SwitchStateChangeNotifier(Notifier& notifier);
    ~SwitchStateChangeNotifier() override;

    void observe(const tfw::hal::buttons::ButtonStateChangedEvent& event) override;

private:
    Notifier& notifier;
};
