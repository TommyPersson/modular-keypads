#pragma once

#include "Notifier.h"
#include "firmwares/common/monitors/RotationalEncoderMonitor.h"
#include <tfw/utils/observables.h>

class EncoderRotationNotifier : public tfw::utils::observables::Observer<EncoderRotatedEvent> {
public:
    explicit EncoderRotationNotifier(Notifier& notifier);
    ~EncoderRotationNotifier() override;

    void observe(const EncoderRotatedEvent& event) override;

private:
    Notifier& notifier;
};
