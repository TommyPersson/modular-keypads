#pragma once

#include "Notifier.h"
#include "firmwares/common/monitors/SwitchMonitor.h"
#include <tfw/utils/observables.h>

class SwitchStateChangeNotifier : public tfw::utils::observables::Observer<SwitchEvent> {
public:
    explicit SwitchStateChangeNotifier(Notifier& notifier);
    ~SwitchStateChangeNotifier() override;

    void observe(const SwitchEvent& event) override;

private:
    Notifier& notifier;
};
