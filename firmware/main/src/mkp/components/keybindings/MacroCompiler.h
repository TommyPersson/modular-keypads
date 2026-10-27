#pragma once

#include <memory>
#include <vector>

#include <tfw/hal/random.h>
#include <tfw/hal/usb/USBAction.h>

#include "mkp/components/macros/Macro.h"

namespace mkp::components::keybindings {
    struct CompiledMacro {
        uint16_t macroId = 0;
        std::vector<std::shared_ptr<tfw::hal::usb::Action>> actions = {};
        // The steps of a sequence, which runs other macros rather than actions of its own
        std::vector<macros::SequenceStep> steps = {};
        // Sent when the button that ran the actions is released, such as to end a MIDI note
        std::vector<std::shared_ptr<tfw::hal::usb::Action>> releaseActions = {};
    };

    // Turns stored macro data into the actions that are sent when the macro is executed. Results in null for macro
    // types that can't be executed. Random text is generated from the random source each time the actions are sent, so
    // the source must outlive the result.
    std::shared_ptr<CompiledMacro> compileMacro(
        const macros::Macro& macro,
        tfw::hal::random::RandomSource& randomSource
    );

    // The control change value that represents the relative change by the amount, in the encoding
    uint8_t encodeRelativeControlChange(int8_t amount, macros::RelativeControlChangeEncoding encoding);
}
