#include "MacroDataSerializers.h"

#include "serializers/ConsumerControlMacroDataSerializer.h"
#include "serializers/MidiAbsoluteControlChangeMacroDataSerializer.h"
#include "serializers/MidiNoteMacroDataSerializer.h"
#include "serializers/MidiRelativeControlChangeMacroDataSerializer.h"
#include "serializers/MouseButtonMacroDataSerializer.h"
#include "serializers/MouseMovementMacroDataSerializer.h"
#include "serializers/MouseScrollMacroDataSerializer.h"
#include "serializers/RandomTextMacroDataSerializer.h"
#include "serializers/SequenceMacroDataSerializer.h"
#include "serializers/ShortcutMacroDataSerializer.h"
#include "serializers/SystemControlMacroDataSerializer.h"
#include "serializers/TextMacroDataSerializer.h"

namespace mkp::components::macros {
    std::vector<void*> macroDataSerializers{
        new ShortcutMacroDataSerializer{},
        new ConsumerControlMacroDataSerializer{},
        new SystemControlMacroDataSerializer{},
        new TextMacroDataSerializer{},
        new MouseScrollMacroDataSerializer{},
        new MouseButtonMacroDataSerializer{},
        new MouseMovementMacroDataSerializer{},
        new SequenceMacroDataSerializer{},
        new MidiNoteMacroDataSerializer{},
        new MidiRelativeControlChangeMacroDataSerializer{},
        new MidiAbsoluteControlChangeMacroDataSerializer{},
        new RandomTextMacroDataSerializer{},
    };
}
