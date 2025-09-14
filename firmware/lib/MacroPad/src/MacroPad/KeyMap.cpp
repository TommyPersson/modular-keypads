#include "KeyMap.h"

KeyMap::KeyMap(const std::vector<KeyBinding>& bindings) {
    for (const auto& binding : bindings) {
        this->bindings.push_back(std::make_shared<KeyBinding>(binding));
    }
}

std::shared_ptr<KeyBinding> KeyMap::getKeyBinding(const KeySwitch& keySwitch) const {
    for (const auto& binding : this->bindings) {
        if (binding->row == keySwitch.row && binding->col == keySwitch.col) {
            return binding;
        }
    }

    return nullptr;
}
