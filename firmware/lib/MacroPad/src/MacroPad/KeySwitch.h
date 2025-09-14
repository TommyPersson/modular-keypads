#pragma once

// ReSharper disable once CppUnusedIncludeDirective // false alarm
#include <cstdint>

#include "Pins/InputPin.h"
#include "Pins/OutputPin.h"

struct KeySwitch {
    InputPin& rowPin;
    OutputPin& colPin;
    int row;
    int col;
};
