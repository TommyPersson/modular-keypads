#pragma once

#include "Notifier.h"
#include "firmwares/common/monitors/SwitchMonitor.h"
#include "utils/observables/Observable.h"

class SwitchStateChangeNotifier : public Observer<SwitchEvent> {
public:
    explicit SwitchStateChangeNotifier(Notifier& notifier);
    ~SwitchStateChangeNotifier() override;

    void observe(const SwitchEvent& event) override;

private:

    Notifier& notifier;
};
