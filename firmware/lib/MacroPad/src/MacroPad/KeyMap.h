#pragma once

#include <vector>
#include <memory>

#include "KeyBinding.h"
#include "KeySwitch.h"

class KeyMap {
public:
    explicit KeyMap(const std::vector<KeyBinding>& bindings);

    std::shared_ptr<KeyBinding> getKeyBinding(const KeySwitch& keySwitch) const;

private:
    std::vector<std::shared_ptr<KeyBinding>> bindings;
};
