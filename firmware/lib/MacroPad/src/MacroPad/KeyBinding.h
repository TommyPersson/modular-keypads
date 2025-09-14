#pragma once

#include <memory>

#include "KeyBindingAction.h"

struct KeyBinding {
    int row;
    int col;
    std::shared_ptr<KeyBindingAction> action;
};
