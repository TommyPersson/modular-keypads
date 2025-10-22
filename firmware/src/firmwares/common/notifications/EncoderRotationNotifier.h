#pragma once

#include "Notifier.h"
#include "firmwares/common/monitors/RotationalEncoderMonitor.h"
#include "utils/observables/Observable.h"

class EncoderRotationNotifier : public Observer<EncoderRotatedEvent> {
public:
    explicit EncoderRotationNotifier(Notifier& notifier);
    ~EncoderRotationNotifier() override;

    void observe(const EncoderRotatedEvent& event) override;

private:
    Notifier& notifier;
};
