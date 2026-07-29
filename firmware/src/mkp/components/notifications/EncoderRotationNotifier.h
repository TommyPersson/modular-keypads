#pragma once

#include <tfw/hal/encoders/RotaryEncoder.h>
#include <tfw/utils/observables.h>
#include "Notifier.h"

class EncoderRotationNotifier : public tfw::utils::observables::Observer<tfw::hal::encoders::EncoderRotatedEvent> {
public:
    explicit EncoderRotationNotifier(Notifier& notifier);
    ~EncoderRotationNotifier() override;

    void observe(const tfw::hal::encoders::EncoderRotatedEvent& event) override;

private:
    Notifier& notifier;
};
