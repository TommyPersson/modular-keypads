#include "KeyBindingStorage.h"

#include <firmwares/common/logging/Logger.h>

using namespace common::keybindings;

namespace {
    auto logger = common::logging::createLogger("KeyBindingStorage");

    std::vector<KeyBinding> storage;
}

void KeyBindingStorage::setup() {}

error_t KeyBindingStorage::write(const KeyBinding& keyBinding) {
    remove(*keyBinding.trigger);

    storage.push_back(keyBinding);

    return 0;
}

error_t KeyBindingStorage::remove(const Trigger& trigger) {
   auto foundIndex = -1;
   for (int i = 0; i < storage.size(); ++i) {
       auto& keyBinding = storage[i];
       if (*keyBinding.trigger == trigger) {
           foundIndex = i;
       }
   }

    if (foundIndex >= 0) {
        storage.erase(storage.begin() + foundIndex);
    }

    return 0;
}

void KeyBindingStorage::forEach(const std::function<void(const KeyBinding&)>& callback) {
    for (auto& keyBinding : storage) {
        callback(keyBinding);
    }
}

